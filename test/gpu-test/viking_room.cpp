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

    layout(std430, set = 2, binding = 0) buffer Model {
        mat4 vp[];
    } model;

    layout(std140, set = 0, binding = 0) uniform Camera {
        mat4 view;
        mat4 proj;
        vec3 position;
    } camera;

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

    layout(set = 2, binding = 1) uniform sampler2D texture_sampler;
    layout(set = 2, binding = 2) uniform Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    } material;

    layout(set = 1, binding = 0) uniform DirectionLight {
        vec3 direction;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    } environment_light;

    void main() {
        vec3 ambient = environment_light.ambient * material.ambient;

        vec3 normal = normalize(frag_normal);
        vec3 light_direction = normalize(-environment_light.direction);
        float diff = max(dot(normal, light_direction), 0.0);
        vec3 diffuse = environment_light.diffuse * (diff * material.diffuse);

        vec3 view_dir = normalize(frag_camera_position - frag_position);
        vec3 reflect_dir = reflect(-light_direction, normal);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
        vec3 specular = environment_light.specular * (spec * material.specular);

        vec3 color = (ambient + diffuse + specular) * texture(texture_sampler, frag_uv).xyz;
        out_color = vec4(color, 1.0);
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
    alignas(16) PWEngine::Vec3<float> position;

    alignas(16) PWEngine::Vec3<float> ambient;
    alignas(16) PWEngine::Vec3<float> diffuse;
    alignas(16) PWEngine::Vec3<float> specular;
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

        auto image = std::make_unique<PWEngine::File::ImageRGBA8>(PWEngine::File::TGA{}, p_instance->log, "./assets/viking_room.tga");
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


struct ScenePipeline
{
    PWEngine::GPU::GPUInstance* p_instance;
    
    std::unique_ptr<Mesh3D> model_mesh;

    std::unique_ptr<PWEngine::GPU::Pipeline> pipeline;

    std::unique_ptr<PWEngine::GPU::BindingLayout> uniform_camera_binding_layout;
    std::unique_ptr<PWEngine::GPU::BindingLayout> uniform_environment_light_binding_layout;


    std::unique_ptr<PWEngine::GPU::GPUBinding> uniform_camera_buffer;
    std::unique_ptr<PWEngine::GPU::Binding> uniform_camera_binding;


    std::unique_ptr<PWEngine::GPU::GPUBinding> uniform_environment_light_buffer;
    std::unique_ptr<PWEngine::GPU::Binding> uniform_environment_light_binding;

    void createLayout()
    {
        std::vector<PWEngine::GPU::BindingInfo> uniform_camera_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
        };
        this->uniform_camera_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, uniform_camera_binding_info);

        std::vector<PWEngine::GPU::BindingInfo> uniform_environment_light_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
        };
        this->uniform_environment_light_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, uniform_environment_light_binding_info);
    }

    void createAsset()
    {
        this->uniform_camera_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(CameraData));
        this->uniform_camera_binding = this->uniform_camera_binding_layout->createBinding();
        this->uniform_camera_binding->update(0, {uniform_camera_buffer.get()});

        this->uniform_environment_light_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(DirectionLight));
        this->uniform_environment_light_binding = this->uniform_environment_light_binding_layout->createBinding();
        this->uniform_environment_light_binding->update(0, {uniform_environment_light_buffer.get()});
    }

    void createPipeline()
    {
        PWEngine::GPU::PipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(PWEngine::File::ObjVertex3D)}},
            {
                {0, 0, offsetof(PWEngine::File::ObjVertex3D, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 1, offsetof(PWEngine::File::ObjVertex3D, normal), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 2, offsetof(PWEngine::File::ObjVertex3D, uv), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
            },
            {
                this->uniform_camera_binding_layout.get(), this->uniform_environment_light_binding_layout.get(), 
                this->model_mesh->layout.get(),
            },
            {{0, sizeof(float), PWEngine::GPU::ShaderType::Fragment}},
            PWEngine::GPU::PipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::PipelineInfo::CullMode::Front,
            std::nullopt,
            PWEngine::GPU::DepthStencilInfo{},
            PWEngine::GPU::RenderingInfo{},
        };
        this->pipeline = std::make_unique<PWEngine::GPU::Pipeline>(this->p_instance, pipeline_info);        
    }

    ScenePipeline(PWEngine::GPU::GPUInstance* instance)
    {
        this->p_instance = instance;
        this->model_mesh = std::make_unique<Mesh3D>(instance, "./assets/viking_room.obj", "./assets/viking_room.tga");

        this->createLayout();
        this->createAsset();
        this->createPipeline();
    }
        
};

static inline void cameraMoveFromLook(PWEngine::Vec2<float> step, PWEngine::Vec3<float>& position, PWEngine::Vec3<float>& look_vector)
{
    position.x += step.x * look_vector.x;
    position.y += step.x * look_vector.y;
    position.z += step.x * look_vector.z;

    position.x += step.y * look_vector.y;
    position.y -= step.y * look_vector.x;
    position.z -= step.y * look_vector.z;
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
            std::clamp(look_degree.x - (float)(dm.y*0.1), -89.0f , 89.0f),
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
            PWEngine::perspective(PWEngine::deg2rad(60.0), this->m_window->getWindowSize().x / (float) this->m_window->getWindowSize().y, 0.01 , 100),
            position
        };
        camera_data.proj.rc(1, 1) *= -1;
    } 
    bool loop(double delta) override
    {  
        if (this->m_window->input != nullptr) this->camera();
        this->scene_pipeline->uniform_camera_buffer->writeData(&camera_data, sizeof(CameraData));

        DirectionLight env_light = {
            {-0.2, -0.3, -1},
            {0.2, 0.2, 0.2},
            {0.5, 0.5, 0.5},
            {1, 1, 1}
        };
        this->scene_pipeline->uniform_environment_light_buffer->writeData(&env_light, sizeof(DirectionLight));

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

        this->recordCommand([&](PWEngine::GPU::RecordCommandWithWindow* cmd){
            cmd->setScissor();
            cmd->setViewPort();
            cmd->bindPipeline(this->scene_pipeline->pipeline.get(), PWEngine::GPU::PipelineType::Render);

            cmd->Rendering({0, 0, 0, 1},true, [&](PWEngine::GPU::RenderingCommand* rendering){
                rendering->bindBinding(this->scene_pipeline->pipeline.get(), {
                    this->scene_pipeline->uniform_camera_binding.get(), this->scene_pipeline->uniform_environment_light_binding.get(), 
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