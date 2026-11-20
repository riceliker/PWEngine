#include "file/tga.hpp"
#include "format.hpp"
#include "gpu.hpp"
#include "file/obj.hpp"
#include "math.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <memory>
#include <string>
#include <vector>

static const std::string scene_vertex_shader_code = R"(
    #version 450

    layout(location = 0) in vec3 in_position;
    layout(location = 1) in vec3 in_normal;
    layout(location = 2) in vec2 in_uv;

    layout(location = 0) out vec3 frag_normal;
    layout(location = 1) out vec2 frag_uv;

    layout(set = 0, binding = 0) uniform Camera {
        mat4 view;
        mat4 proj;
    } camera;

    layout(std430, set = 1, binding = 1) readonly buffer Model {
        mat4 vp[];
    } model;

    void main() {
        gl_Position = camera.proj * camera.view * model.vp[gl_InstanceIndex] * vec4(in_position, 1.0);
        frag_normal = in_normal;
        frag_uv = in_uv;
    }   
)";

static const std::string scene_fragment_shader_code = R"(

    #version 450

    layout(location = 0) in vec3 frag_normal;
    layout(location = 1) in vec2 frag_uv;

    layout(location = 0) out vec4 out_color;

    layout(set = 1, binding = 0) uniform sampler2D texture_sampler;

    void main() {
        out_color = texture(texture_sampler, frag_uv);
    }
)";

struct CameraData
{
    PWEngine::Mat4 view;
    PWEngine::Mat4 proj;
};

struct Mesh3D
{
    PWEngine::GPU::GPUInstance* p_instance;

    std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
    std::unique_ptr<PWEngine::GPU::Binding> binding;

    std::unique_ptr<PWEngine::GPU::GPULocalBuffer> vertex_buffer;
    std::unique_ptr<PWEngine::GPU::GPULocalBuffer> index_buffer;
    size_t vertex_count = 0;

    std::unique_ptr<PWEngine::GPU::GPUBindingBuffer> storage_model_buffer;

    std::unique_ptr<PWEngine::GPU::GPUImage> image;
    std::unique_ptr<PWEngine::GPU::GPUImageView> image_view;

    Mesh3D(PWEngine::GPU::GPUInstance* instance, std::string model_path, std::string texture_path)
    {
        this->p_instance = instance;
        std::vector<PWEngine::GPU::BindingInfo> layout_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::image_sampler},
            {1, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::storage},
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, layout_binding_info);

        auto [model, model_err] = PWEngine::Obj::ObjFile(model_path);
        PWEngine::sendLog(this->p_instance->log, PWEngine::LogType::Info, PWEngine::LogFrom::FileIO, std::format("Open obj file: Path:{}, Result: {}", model_path, static_cast<uint8_t>(model_err)));
        this->vertex_buffer = std::make_unique<PWEngine::GPU::GPULocalBuffer>(p_instance, PWEngine::GPU::BufferUsage::vertex, sizeof(PWEngine::Format::Vertex3D) * model->vertices.size(), model->vertices.data());
        this->index_buffer = std::make_unique<PWEngine::GPU::GPULocalBuffer>(p_instance, PWEngine::GPU::BufferUsage::index,sizeof(uint32_t) * model->indices.size(), model->indices.data());
        this->vertex_count = model->vertices.size();

        auto [image, image_err] = PWEngine::Tga::TgaFile(texture_path);
                PWEngine::sendLog(this->p_instance->log, PWEngine::LogType::Info, PWEngine::LogFrom::FileIO, std::format("Open tga file: Path:{}, Result: {}", texture_path, static_cast<uint8_t>(image_err)));
        this->image = std::make_unique<PWEngine::GPU::GPUImage>(p_instance, image.get(), true, true);

        this->storage_model_buffer = std::make_unique<PWEngine::GPU::GPUBindingBuffer>(p_instance, PWEngine::GPU::BufferUsage::storage, sizeof(PWEngine::Mat4));
        this->image_view = std::make_unique<PWEngine::GPU::GPUImageView>(this->image.get());

        this->binding = this->layout->createBinding();
        this->binding->update(0, {this->image_view.get(), this->storage_model_buffer.get()});
    }

    void setData(std::vector<PWEngine::Mat4>* transform)
    {
        memcpy(this->storage_model_buffer->map_pointer, transform->data(), transform->size() * sizeof(PWEngine::Mat4));
    }
};

static inline void cameraMoveFromLook(PWEngine::Vec2<float> step, PWEngine::Vec3<float>& position, PWEngine::Vec3<float>& look_vector)
{
    position.x += step.x * look_vector.x;
    position.y += step.x * look_vector.y;

    position.x += step.y * look_vector.y;
    position.y -= step.y * look_vector.x;
}

struct Camera
{
    PWEngine::GPU::GPUInstance* p_instance;
    std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
    std::unique_ptr<PWEngine::GPU::GPUBindingBuffer> uniform_camera_buffer;
    std::unique_ptr<PWEngine::GPU::Binding> binding;

    bool is_in_screen = false;
    PWEngine::Vec2<float> last_mouse = {0 , 0};
    PWEngine::Vec2<float> look_degree = {0, 0};
    PWEngine::Vec3<float> position = {0, 0, 0};
    CameraData camera_data = {PWEngine::Mat4(1), PWEngine::Mat4(1)};

    Camera()
    {
        std::vector<PWEngine::GPU::BindingInfo> binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, binding_info);

        this->uniform_camera_buffer = std::make_unique<PWEngine::GPU::GPUBindingBuffer>(p_instance, PWEngine::GPU::BufferUsage::uniform, sizeof(CameraData));
        this->binding = this->layout->createBinding();
        this->binding->update(0, {uniform_camera_buffer.get()});
    }
    void updateCamera(PWEngine::GPU::Window* window)
    {       
        if (window->input->is_hover && window->input->mouse_key[static_cast<uint8_t>(PWEngine::GPU::MouseKey::BtnLeft)])
        {
            this->p_instance->setMouseCursor(PWEngine::GPU::MouseInputMode::disable);
            is_in_screen = true;
            last_mouse = { (float)window->input->mouse_position_x.load(), (float)window->input->mouse_position_y.load()};
        }

        if (window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::Escape)])
        {
            this->p_instance->setMouseCursor(PWEngine::GPU::MouseInputMode::show);
            is_in_screen = false;
        }
            
        PWEngine::Vec2<float> current_mouse = last_mouse;

        PWEngine::Vec2<float> dm{0,0};
        if (is_in_screen)
        {
            current_mouse = { (float)window->input->mouse_position_x.load(), (float)window->input->mouse_position_y.load()};
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
        if (window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::W)])
        {
            cameraMoveFromLook({speed, 0}, position, look_vector);
        } 
        if (window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::S)]) 
        {
            cameraMoveFromLook({-speed, 0}, position, look_vector);
        }
        if (window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::A)])
        {
            cameraMoveFromLook({0, -speed}, position, look_vector);
        }
        if (window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::D)]) 
        {
            cameraMoveFromLook({0, speed}, position, look_vector);
        }  

        if (window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::Up)])
        {
            position.z += speed;
        }
        if (window->input->key_list[static_cast<uint16_t>(PWEngine::GPU::Key::Down)])
        {
            position.z -= speed;
        }

        camera_data = {
            PWEngine::look(position, position + look_vector, PWEngine::Vec3<float>(0.0f, 0.0f, 1.0f)),
            PWEngine::perspective(PWEngine::deg2rad(60.0), window->getWindowSize().x / (float) window->getWindowSize().y, 0.01 , 100)
        };
        camera_data.proj.rc(1, 1) *= -1;

        memcpy(this->uniform_camera_buffer->map_pointer, &camera_data, sizeof(CameraData));
    } 
};

struct ScenePipeline
{
    PWEngine::GPU::GPUInstance* p_instance;

    std::unique_ptr<Mesh3D> ground;
    std::unique_ptr<Mesh3D> model_mesh;
    std::unique_ptr<Camera> camera;

    std::unique_ptr<PWEngine::GPU::RenderPipeline> pipeline;

    ScenePipeline(PWEngine::GPU::GPUInstance* instance)
    {
        this->p_instance = instance;
        this->model_mesh = std::make_unique<Mesh3D>(instance, "./assets/viking_room.obj", "./assets/viking_room.tga");
        this->ground = std::make_unique<Mesh3D>(instance, "./assets/box.obj", "./assets/ground.tga");

        PWEngine::GPU::RenderPipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(scene_vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(scene_fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(PWEngine::Format::Vertex3D)}},
            {
                {0, 0, offsetof(PWEngine::Format::Vertex3D, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 1, offsetof(PWEngine::Format::Vertex3D, normal), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 2, offsetof(PWEngine::Format::Vertex3D, uv), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
            },
            {
                this->camera->layout.get(),
                this->model_mesh->layout.get(),
            },
            {{0, sizeof(float), PWEngine::GPU::ShaderType::Fragment}},
            PWEngine::GPU::RenderPipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::RenderPipelineInfo::CullMode::Front,
            std::nullopt,
            PWEngine::GPU::DepthStencilInfo{},
        };
        this->pipeline = std::make_unique<PWEngine::GPU::RenderPipeline>(this->p_instance, pipeline_info);        
    } 
};

