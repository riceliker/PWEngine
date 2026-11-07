#include "gpu.hpp"
#include "file.hpp"
#include "math.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

static const std::string vertex_shader_code = R"(
    #version 450

    layout(location = 0) in vec3 in_position;
    layout(location = 1) in vec3 in_normal;
    layout(location = 2) in vec2 in_uv;

    layout(location = 0) out vec3 frag_normal;
    layout(location = 1) out vec2 frag_uv;
    layout(location = 2) out vec3 frag_position;
    layout(location = 3) out vec3 frag_camera_position;

    layout(std140, set = 0, binding = 0) uniform Camera {
        mat4 view;
        mat4 proj;
        vec3 position;
    } camera;

    layout(std430, set = 2, binding = 0) buffer Model {
        mat4 vp[];
    } model;

    void main() {
        gl_Position = camera.proj * camera.view * model.vp[gl_InstanceIndex] * vec4(in_position, 1.0);
        frag_normal = in_normal;
        frag_uv = in_uv;
        frag_position = vec4(model.vp[gl_InstanceIndex] * vec4(in_position, 1.0)).xyz;
        frag_camera_position = camera.position;
    }   
)";

static const std::string fragment_shader_code = R"(

    #version 450

    layout(location = 0) in vec3 frag_normal;
    layout(location = 1) in vec2 frag_uv;
    layout(location = 2) in vec3 frag_position;
    layout(location = 3) in vec3 frag_camera_position;

    layout(location = 0) out vec4 out_color;

    layout(set = 1, binding = 0) uniform DirectLight {
        vec3 direction;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    } direct_light;

    struct PointLight {
        vec3 position;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float constant;
        float linear;
        float quadratic;
    };
    layout(set = 1, binding = 1) buffer PointLightStorage {
        PointLight lights[];
    } point_lights;


    layout(set = 2, binding = 1) uniform sampler2D texture_sampler;
    layout(set = 2, binding = 2) uniform Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    } material;

    vec3 CalDirectLight(vec3 normal, vec3 view_direct) {
        vec3 light_direct = normalize(-direct_light.direction);
        float diff = dot(normal, light_direct);
        diff = diff * 0.5 + 0.5;
        vec3 reflect_direct = reflect(-light_direct, normal);
        float spec = pow(max(dot(view_direct, reflect_direct), 0), material.shininess);
        vec3 ambient  = direct_light.ambient  * vec3(texture(texture_sampler, frag_uv));
        vec3 diffuse  = direct_light.diffuse  * diff * vec3(texture(texture_sampler, frag_uv));
        vec3 specular = direct_light.specular * spec * vec3(texture(texture_sampler, frag_uv));
        return (ambient + diffuse + specular);
    }

    vec3 CalPointLight(PointLight point_light, vec3 normal, vec3 frag_position, vec3 view_direct) {
        vec3 light_direct = normalize(point_light.position - frag_position);
        float diff = dot(normal, light_direct);
        diff = diff * 0.5 + 0.5;
        vec3 reflect_direct = reflect(-light_direct, normal);
        float spec = pow(max(dot(view_direct, reflect_direct), 0), material.shininess);
        float distance    = length(point_light.position - frag_position);
        float attenuation = 1.0 / (point_light.constant + point_light.linear * distance + 
                    point_light.quadratic * (distance * distance));    

        vec3 ambient  = point_light.ambient * vec3(texture(texture_sampler, frag_uv));
        vec3 diffuse  = point_light.diffuse * diff * vec3(texture(texture_sampler, frag_uv));
        vec3 specular = point_light.specular * spec * vec3(texture(texture_sampler, frag_uv));
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
    }

    void main() {
        vec3 norm = normalize(frag_normal);
        vec3 view_direct = normalize(frag_camera_position - frag_position);
        vec3 result = CalDirectLight(norm, view_direct);

        for(int i = 0; i < 1; i++)
            result += CalPointLight(point_lights.lights[i], norm, frag_position, view_direct);  

        out_color = vec4(result, 1.0);
    }
)";

struct CameraData
{
    PWEngine::Mat4 view;
    PWEngine::Mat4 proj;
    alignas(16) PWEngine::Vec3<float> position;
};

struct DirectionLight
{
    alignas(16) PWEngine::Vec3<float> direction;

    alignas(16) PWEngine::Vec3<float> ambient;
    alignas(16) PWEngine::Vec3<float> diffuse;
    alignas(16) PWEngine::Vec3<float> specular;
};

struct PointLight
{
    alignas(16) PWEngine::Vec3<float> position;

    alignas(16) PWEngine::Vec3<float> ambient;
    alignas(16) PWEngine::Vec3<float> diffuse;
    alignas(16) PWEngine::Vec3<float> specular;

    float constant;
    float linear;
    float quadratic;
    float __padding;
};

struct Material
{
    alignas(16) PWEngine::Vec3<float> ambient;
    alignas(16) PWEngine::Vec3<float> diffuse;
    alignas(16) PWEngine::Vec3<float> specular;
    alignas(16) float shininess;
};

struct Mesh3D
{
    PWEngine::GPU::GPUInstance* p_instance;

    std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
    std::unique_ptr<PWEngine::GPU::Binding> binding;

    std::unique_ptr<PWEngine::GPU::GPUVertex> vertex_buffer;
    std::unique_ptr<PWEngine::GPU::GPUVertex> index_buffer;
    size_t vertex_count = 0;


    std::unique_ptr<PWEngine::GPU::GPUBinding> storage_model_buffer;

    std::unique_ptr<PWEngine::GPU::GPUImage> image;
    std::unique_ptr<PWEngine::GPU::GPUImageView> image_view;

    std::unique_ptr<PWEngine::GPU::GPUBinding> uniform_material_buffer;

    Mesh3D(PWEngine::GPU::GPUInstance* instance, std::string model_path, std::string texture_path)
    {
        this->p_instance = instance;
        std::vector<PWEngine::GPU::BindingInfo> layout_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::storage},
            {1, 1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::image_sampler},
            {2, 1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::uniform}
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, layout_binding_info);

        auto model = PWEngine::File::objReader(p_instance->log, model_path);
        this->vertex_buffer = std::make_unique<PWEngine::GPU::GPUVertex>(p_instance, PWEngine::GPU::GPUVertexDataType::vertex, model->vertices);
        this->index_buffer = std::make_unique<PWEngine::GPU::GPUVertex>(p_instance, PWEngine::GPU::GPUVertexDataType::index, model->indices);
        this->vertex_count = model->vertices.size();

        auto image = std::make_unique<PWEngine::File::ImageRGBA8>(PWEngine::File::TGA{}, p_instance->log, texture_path);
        this->image = std::make_unique<PWEngine::GPU::GPUImage>(p_instance, image.get());

        this->storage_model_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::storage, sizeof(PWEngine::Mat4));
        this->image_view = std::make_unique<PWEngine::GPU::GPUImageView>(this->image.get());
        this->uniform_material_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(Material));

        this->binding = this->layout->createBinding();
        this->binding->update(0, {this->storage_model_buffer.get(), this->image_view.get(), this->uniform_material_buffer.get()});
    }

    void setData(std::vector<PWEngine::Mat4>& transform, Material& material)
    {
        this->storage_model_buffer->writeData(transform.data(), sizeof(PWEngine::Mat4) * transform.size());
        this->uniform_material_buffer->writeData(&material, sizeof(Material));
    }
};

struct Light
{
    PWEngine::GPU::GPUInstance* p_instance;

    std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
    std::unique_ptr<PWEngine::GPU::Binding> binding;

    std::unique_ptr<PWEngine::GPU::GPUBinding> uniform_direct_buffer;

    std::unique_ptr<PWEngine::GPU::GPUBinding> storage_point_buffer;

    Light(PWEngine::GPU::GPUInstance* instance)
    {
        this->p_instance = instance;

        std::vector<PWEngine::GPU::BindingInfo> layout_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::uniform},
            {1,  1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::storage},
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, layout_binding_info);

        this->uniform_direct_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(DirectionLight));
        this->storage_point_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::storage, sizeof(PointLight));

        this->binding = this->layout->createBinding();
        this->binding->update(0, {this->uniform_direct_buffer.get(), this->storage_point_buffer.get()});
    }

    void setData(DirectionLight& direct_light, std::vector<PointLight>& point_lights)
    {
        this->uniform_direct_buffer->writeData(&direct_light, sizeof(DirectionLight));
        this->storage_point_buffer->writeData(point_lights.data(), sizeof(PointLight) * point_lights.size());
    }
};


struct ScenePipeline
{
    PWEngine::GPU::GPUInstance* p_instance;
    
    std::unique_ptr<Light> light;
    std::unique_ptr<Mesh3D> ground;
    std::unique_ptr<Mesh3D> model_mesh;

    std::unique_ptr<PWEngine::GPU::RenderPipeline> pipeline;

    std::unique_ptr<PWEngine::GPU::BindingLayout> uniform_camera_binding_layout;
    std::unique_ptr<PWEngine::GPU::BindingLayout> uniform_environment_light_binding_layout;


    std::unique_ptr<PWEngine::GPU::GPUBinding> uniform_camera_buffer;
    std::unique_ptr<PWEngine::GPU::Binding> uniform_camera_binding;

    void createLayout()
    {
        std::vector<PWEngine::GPU::BindingInfo> uniform_camera_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
        };
        this->uniform_camera_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, uniform_camera_binding_info);
    }

    void createAsset()
    {
        this->uniform_camera_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(CameraData));
        this->uniform_camera_binding = this->uniform_camera_binding_layout->createBinding();
        this->uniform_camera_binding->update(0, {uniform_camera_buffer.get()});
    }

    void createPipeline()
    {
        PWEngine::GPU::RenderPipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(PWEngine::File::ObjVertex3D)}},
            {
                {0, 0, offsetof(PWEngine::File::ObjVertex3D, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 1, offsetof(PWEngine::File::ObjVertex3D, normal), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 2, offsetof(PWEngine::File::ObjVertex3D, uv), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
            },
            {
                this->uniform_camera_binding_layout.get(),
                this->light->layout.get(),
                this->model_mesh->layout.get(),
            },
            {{0, sizeof(float), PWEngine::GPU::ShaderType::Fragment}},
            PWEngine::GPU::RenderPipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::RenderPipelineInfo::CullMode::Front,
            std::nullopt,
            PWEngine::GPU::DepthStencilInfo{},
            PWEngine::GPU::RenderingInfo{},
        };
        this->pipeline = std::make_unique<PWEngine::GPU::RenderPipeline>(this->p_instance, pipeline_info);        
    }

    ScenePipeline(PWEngine::GPU::GPUInstance* instance)
    {
        this->p_instance = instance;
        this->model_mesh = std::make_unique<Mesh3D>(instance, "./assets/box.obj", "./assets/icon.tga");
        this->ground = std::make_unique<Mesh3D>(instance, "./assets/box.obj", "./assets/ground.tga");
        this->light = std::make_unique<Light>(instance);

        this->createLayout();
        this->createAsset();
        this->createPipeline();
    }
        
};

static inline void cameraMoveFromLook(PWEngine::Vec2<float> step, PWEngine::Vec3<float>& position, PWEngine::Vec3<float>& look_vector)
{
    position.x += step.x * look_vector.x;
    position.y += step.x * look_vector.y;

    position.x += step.y * look_vector.y;
    position.y -= step.y * look_vector.x;
}

class MyTask : public PWEngine::GPU::GPUTaskWithWindow
{
private:
    std::unique_ptr<ScenePipeline> scene_pipeline;
    bool is_in_screen = false;
    PWEngine::Vec2<float> last_mouse = {0 , 0};
    PWEngine::Vec2<float> look_degree = {0, 0};
    PWEngine::Vec3<float> position = {0, 0, 0};
    CameraData camera_data = {PWEngine::Mat4(1), PWEngine::Mat4(1)};
public:
    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo window_info) : PWEngine::GPU::GPUTaskWithWindow(instance, window_info)
    {
        this->scene_pipeline = std::make_unique<ScenePipeline>(instance);
    }
    void camera()
    {       
        if (this->m_window->input->is_hover && this->m_window->input->mouse_key[static_cast<uint8_t>(PWEngine::GPU::MouseKey::BtnLeft)])
        {
            this->p_instance->setMouseCursor(PWEngine::GPU::MouseInputMode::disable);
            is_in_screen = true;
            last_mouse = { (float)this->m_window->input->mouse_position_x.load(), (float)this->m_window->input->mouse_position_y.load()};
        }

        if (this->m_window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::Escape)])
        {
            this->p_instance->setMouseCursor(PWEngine::GPU::MouseInputMode::show);
            is_in_screen = false;
        }
            
        PWEngine::Vec2<float> current_mouse = last_mouse;

        PWEngine::Vec2<float> dm{0,0};
        if (is_in_screen)
        {
            current_mouse = { (float)this->m_window->input->mouse_position_x.load(), (float)this->m_window->input->mouse_position_y.load()};
            dm = current_mouse - last_mouse;
            last_mouse = current_mouse;
        }

        look_degree = {
            std::clamp(look_degree.x - (float)(dm.y*0.1), -89.9999f , 89.9999f),
            look_degree.y - (float)(dm.x*0.1)
        };

        last_mouse = current_mouse;

        PWEngine::Vec3<float> look_vector = PWEngine::Vec3<float>(
            cos(PWEngine::deg2rad(look_degree.y)),
            sin(PWEngine::deg2rad(look_degree.y)),
            sin(PWEngine::deg2rad(look_degree.x))
        );

        float speed = 0.04;
        if (this->m_window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::W)])
        {
            cameraMoveFromLook({speed, 0}, position, look_vector);
        } 
        if (this->m_window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::S)]) 
        {
            cameraMoveFromLook({-speed, 0}, position, look_vector);
        }
        if (this->m_window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::A)])
        {
            cameraMoveFromLook({0, -speed}, position, look_vector);
        }
        if (this->m_window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::D)]) 
        {
            cameraMoveFromLook({0, speed}, position, look_vector);
        }  

        if (this->m_window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::Up)])
        {
            position.z += speed;
        }
        if (this->m_window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::Down)])
        {
            position.z -= speed;
        }

        camera_data = {
            PWEngine::look(position, position + look_vector, PWEngine::Vec3<float>(0.0f, 0.0f, 1.0f)),
            PWEngine::perspective(PWEngine::deg2rad(60.0), this->m_window->getWindowSize().x / (float) this->m_window->getWindowSize().y, 0.1 , 10),
            position
        };
        camera_data.proj.rc(1, 1) *= -1;
    } 

    bool loop(double delta) override
    {  
        if (this->m_window->input != nullptr) this->camera();
        this->scene_pipeline->uniform_camera_buffer->writeData(&camera_data, sizeof(CameraData));

        /* light */
        DirectionLight direct_light = {
            {-0.2, -0.1, -1},
            {0.2, 0.2, 0.2},
            {0.5, 0.5, 0.5},
            {1, 1, 1}
        };

        std::vector<PointLight> point_lights = {
            {
                {0, 0, 5},
                {0.2, 0.2, 0.2},
                {0.5, 0.5, 0.5},
                {1, 1, 1},
                1, 0.09, 0.032
            }
        };
        this->scene_pipeline->light->setData(direct_light, point_lights);

        /* mesh */

        std::vector<PWEngine::Mat4> mats = {
            PWEngine::transform({0, 0, 0}, {1, 1, 1}, PWEngine::euler2quat({0, 0, 0})),
            PWEngine::transform({2, 0, 0}, {1, 1, 1}, PWEngine::euler2quat({0, 0, 90}))
        };
        
        Material material = {
            {1, 0.5, 0.31},
            {1, 0.5, 0.31},
            {0.5, 0.5, 0.5},
            32
        };
        this->scene_pipeline->model_mesh->setData(mats, material);

        std::vector<PWEngine::Mat4> ground_mats = {
            PWEngine::transform({0, 0, -0.02}, {10, 10, 0.02}, PWEngine::euler2quat({0, 0, 0})),
        };
        
        Material ground_material = {
            {1, 0.5, 0.31},
            {1, 0.5, 0.31},
            {0.5, 0.5, 0.5},
            32
        };
        this->scene_pipeline->ground->setData(ground_mats, ground_material);

        this->recordCommand([&](PWEngine::GPU::RecordCommand* cmd){
            cmd->setScissor({0, 0}, this->m_window->getWindowSize());
            cmd->setViewPort({0, 0}, this->m_window->getWindowSize(), {0, 1});
            cmd->bindPipeline(this->scene_pipeline->pipeline.get());

            cmd->Rendering({0, 0, 0, 1}, true, this, [&](PWEngine::GPU::RenderingCommand* rendering){
                cmd->bindBinding(this->scene_pipeline->pipeline.get(), {
                    this->scene_pipeline->uniform_camera_binding.get(),
                    this->scene_pipeline->light->binding.get(), 
                    this->scene_pipeline->ground->binding.get()
                });
                rendering->bindVertices({this->scene_pipeline->ground->vertex_buffer.get()}, {0});
                rendering->bindIndex(this->scene_pipeline->ground->index_buffer.get(), PWEngine::GPU::InputUintType::u32);
                rendering->draw(this->scene_pipeline->ground->vertex_count, ground_mats.size(), 0, 0);


                cmd->bindBinding(this->scene_pipeline->pipeline.get(), {
                    this->scene_pipeline->uniform_camera_binding.get(),
                    this->scene_pipeline->light->binding.get(), 
                    this->scene_pipeline->model_mesh->binding.get()
                });
                rendering->bindVertices({this->scene_pipeline->model_mesh->vertex_buffer.get()}, {0});
                rendering->bindIndex(this->scene_pipeline->model_mesh->index_buffer.get(), PWEngine::GPU::InputUintType::u32);
                rendering->draw(this->scene_pipeline->model_mesh->vertex_count, mats.size(), 0, 0);

            });
        });
        return false;
    }
};


int main()
{
    PWEngine::LogSystem log(true);

    PWEngine::GPU::GPUInstanceInfo instance_info = {
        true, "RectImage", {1, 0, 0}
    };
    PWEngine::GPU::GPUInstance instance(&log, instance_info);

    PWEngine::GPU::WindowInfo window_info = {
        {1280, 720}, 
        "RectImage",
        true
    };
    
    auto main_task_id = instance.createGPUTask<MyTask>(&instance, window_info);

    instance.run(main_task_id);
}