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

    layout(std430, set = 0, binding = 0) buffer Model {
        mat4 vp[];
    } model;

    layout(set = 1, binding = 0) uniform Camera {
        mat4 view;
        mat4 proj;
    } camera;

    void main() {
        gl_Position = camera.proj * camera.view * model.vp[gl_InstanceIndex] * vec4(in_position, 1.0);
        frag_normal = in_normal;
        frag_uv = in_uv;
    }   
)";

static const std::string fragment_shader_code = R"(
    #version 450

    layout(location = 0) in vec3 frag_normal;
    layout(location = 1) in vec2 frag_uv;
    layout(location = 0) out vec4 out_color;

    layout(set = 2, binding = 0) uniform sampler2D texture_sampler;

    void main() {
        out_color = texture(texture_sampler, frag_uv);
    }
)";

struct CameraData
{
    PWEngine::Mat4 view;
    PWEngine::Mat4 proj;
};

struct ScenePipeline
{
    PWEngine::GPU::GPUInstance* p_instance;
    size_t vertex_count = 0;
    std::unique_ptr<PWEngine::GPU::Pipeline> pipeline;

    std::unique_ptr<PWEngine::GPU::BindingLayout> storage_model_binding_layout;
    std::unique_ptr<PWEngine::GPU::BindingLayout> uniform_camera_binding_layout;
    std::unique_ptr<PWEngine::GPU::BindingLayout> sampler_binding_layout;

    std::unique_ptr<PWEngine::GPU::GPUVertex> vertex_buffer;
    std::unique_ptr<PWEngine::GPU::GPUVertex> index_buffer;

    std::unique_ptr<PWEngine::GPU::GPUBinding> storage_model_buffer;
    std::unique_ptr<PWEngine::GPU::Binding> storage_model_binding;

    std::unique_ptr<PWEngine::GPU::GPUBinding> uniform_camera_buffer;
    std::unique_ptr<PWEngine::GPU::Binding> uniform_camera_binding;

    std::unique_ptr<PWEngine::GPU::GPUImage> image;
    std::unique_ptr<PWEngine::GPU::GPUImageView> image_view;
    std::unique_ptr<PWEngine::GPU::Binding> image_binding;

    void createLayout()
    {
        std::vector<PWEngine::GPU::BindingInfo> storage_model_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::storage},
        };
        this->storage_model_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, storage_model_binding_info);

        std::vector<PWEngine::GPU::BindingInfo> uniform_camera_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
        };
        this->uniform_camera_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, uniform_camera_binding_info);

        std::vector<PWEngine::GPU::BindingInfo> sampler_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::image_sampler},
        };
        this->sampler_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(this->p_instance, sampler_binding_info);
    }

    void createAsset()
    {
        auto model = PWEngine::File::objReader(p_instance->log, "./assets/viking_room.obj");
        this->vertex_buffer = std::make_unique<PWEngine::GPU::GPUVertex>(p_instance, PWEngine::GPU::GPUVertexDataType::vertex, model->vertices);
        this->index_buffer = std::make_unique<PWEngine::GPU::GPUVertex>(p_instance, PWEngine::GPU::GPUVertexDataType::index, model->indices);
        this->vertex_count = model->vertices.size();

        auto model_image = std::make_unique<PWEngine::File::ImageRGBA8>(PWEngine::File::TGA{}, p_instance->log, "./assets/viking_room.tga");
        this->image = std::make_unique<PWEngine::GPU::GPUImage>(p_instance, model_image.get());
        this->image_view = std::make_unique<PWEngine::GPU::GPUImageView>(this->image.get());
        this->image_binding = this->sampler_binding_layout->createBinding();
        image_binding->update(0, {this->image_view.get()});

        this->storage_model_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(PWEngine::Mat4));
        this->storage_model_binding = this->storage_model_binding_layout->createBinding();
        this->storage_model_binding->update(0, {storage_model_buffer.get()});

        this->uniform_camera_buffer = std::make_unique<PWEngine::GPU::GPUBinding>(p_instance, PWEngine::GPU::GPUBindingDataType::uniform, 2 * sizeof(PWEngine::Mat4));
        this->uniform_camera_binding = this->uniform_camera_binding_layout->createBinding();
        this->uniform_camera_binding->update(0, {uniform_camera_buffer.get()});
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
            {this->storage_model_binding_layout.get(), this->uniform_camera_binding_layout.get(), this->sampler_binding_layout.get()},
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
    bool loop(double delta) override
    {  
        if (this->m_window->input != nullptr)
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
                PWEngine::perspective(PWEngine::deg2rad(60.0), this->m_window->getWindowSize().x / (float) this->m_window->getWindowSize().y, 0.01 , 100)
            };
            camera_data.proj.rc(1, 1) *= -1;
        }
        this->scene_pipeline->uniform_camera_buffer->writeData(&camera_data, sizeof(CameraData));

        std::vector<PWEngine::Mat4> mats = {
            PWEngine::transform({0, 0, 0}, {1, 1, 1}, {0, 0, 0, 0}),
            PWEngine::transform({2, 0, 0}, {1, 1, 2}, {0, 0.5, 0, 0.5})
        };
        this->scene_pipeline->storage_model_buffer->writeData(mats.data(), sizeof(PWEngine::Mat4) * mats.size());

        this->recordCommand([&](PWEngine::GPU::RecordCommandWithWindow* cmd){
            cmd->setScissor();
            cmd->setViewPort();
            cmd->bindPipeline(this->scene_pipeline->pipeline.get(), PWEngine::GPU::PipelineType::Render);

            cmd->Rendering({0, 0, 0, 1},true, [&](PWEngine::GPU::RenderingCommand& rendering){
                rendering.bindBinding(this->scene_pipeline->pipeline.get(), {this->scene_pipeline->storage_model_binding.get(), this->scene_pipeline->uniform_camera_binding.get(), this->scene_pipeline->image_binding.get()});
                rendering.bindVertices({this->scene_pipeline->vertex_buffer.get()}, {0});
                rendering.bindIndex(this->scene_pipeline->index_buffer.get(), PWEngine::GPU::InputUintType::u32);
                rendering.draw(this->scene_pipeline->vertex_count, mats.size(), 0, 0);
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


    



    
    // auto pipeline3D = context->createPipeline3D({}, {"./shaders/pipeline3D/vert.spv", "./shaders/pipeline3D/frag.spv"});
    // auto mesh = pipeline3D->createMesh3D(model.get());
    // auto mesh_transform = context->createTransform3D();
    // mesh_transform->resizeTransform(2);
    
    // mesh->bindTransform3D(mesh_transform);
    
    // auto material = pipeline3D->createMaterial();
    // material->bindBasicTexture(model_texture);
    // material->updateDescriptor();
    // auto camera = pipeline3D->createCamera();
    // camera->updateDescriptor();

    // auto pipeline_hud = context->createPipelineHUD();
    // auto set = pipeline_hud->createDS();
    
    // set->update(pipeline_hud.get(), icon_texture.get());
    
    // size_t loops = 0;
    // double total = 0;
    // float time = 0;
    // float speed = 0.02;
    // bool is_in_screen = false;
    // PWEngine::Vec2<float> last_mouse = {0, 0};
    // PWEngine::Vec2<float> look_degree = {0, 0};
    // context->frameLoop([&](PWEngine::Render::Command* cmd, PWEngine::Render::Input* input, float delta){

    //     if (input->checkIsHoverScreen() && input->checkIsMouseInput(PWEngine::Render::MouseKey::BtnLeft))
    //     {
    //         context->hideMouseCursor();
    //         is_in_screen = true;
    //         last_mouse = input->checkMousePosition();
    //     }
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::Escape))
    //     {
    //         context->showMouseCursor();
    //         is_in_screen = false;
    //     }
        
    //     auto current_mouse = last_mouse;
        
    //     PWEngine::Vec2<float> dm{0,0};
    //     if (is_in_screen)
    //     {
    //         current_mouse = input->checkMousePosition();
    //         dm = current_mouse - last_mouse;
    //         last_mouse = current_mouse;
    //     }

    //     look_degree = {
    //         std::clamp(look_degree.x - (float)(dm.y*0.1), -89.0f , 89.0f),
    //         look_degree.y - (float)(dm.x*0.1)
    //     };

    //     last_mouse = current_mouse;

    //     camera->calculateLookVector(look_degree);

    //     float speed = 0.04;
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::W))
    //     {
    //         camera->cameraMoveFromLook({speed, 0});
    //     } 
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::S)) 
    //     {
    //         camera->cameraMoveFromLook({-speed, 0});
    //     }
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::A))
    //     {
    //         camera->cameraMoveFromLook({0, -speed});
    //     }
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::D)) 
    //     {
    //         camera->cameraMoveFromLook({0, speed});
    //     }  

    //     cmd->setViewPort();
    //     cmd->setScissor();
    //     cmd->setPipeline(pipeline3D.get());

    //     total += delta;
    //     //PWEngine::sendLog(instance.log, PWEngine::LogType::Debug, PWEngine::LogFrom::Debug, std::to_string(1/delta));
    //     loops++;

    //     camera->updateData(cmd);
    //     mesh_transform->setPosition(0, {0, 0, 0});
    //     mesh_transform->setRotation(0, {0, 0, 0.5});
    //     mesh_transform->setPosition(1, {0, 1.5, 0});
    //     mesh_transform->setScale(1, {1, 1, 2});
    //     mesh_transform->calculateNodeMatrix(cmd);

    //     cmd->renderingBegin({0.52, 0.62, 0.65, 1}, true);
    //     cmd->draw(pipeline3D.get(), mesh.get(), material.get(), camera.get());
    //     cmd->renderingEnd();

    //     cmd->renderingBarrier();
    //     cmd->setViewPort();
    //     cmd->setScissor();

    //     cmd->setPipeline(pipeline_hud.get());

    //     cmd->renderingBegin({0, 0, 0, 0}, false);
    //     cmd->draw(pipeline_hud.get(), {{-0.05, -0.05, 0.1, 0.1}, {0, 0, 1, 1}, PWEngine::Mat4(1)}, icon_texture.get(), set.get());
    //     cmd->renderingEnd();

    // });

    // void Camera3D::calculateLookVector(Vec2<float> look_degree)
    // {
    //     this->look_vector = 
    // }

    // void Camera3D::cameraMoveFromLook(Vec2<float> step)
    // {
    //     this->position.x += step.x * this->look_vector.x;
    //     this->position.y += step.x * this->look_vector.y;
    //     this->position.z += step.x * this->look_vector.z;

    //     this->position.x += step.y * this->look_vector.y;
    //     this->position.y -= step.y * this->look_vector.x;
    //     this->position.z -= step.y * this->look_vector.z;
    // }

}