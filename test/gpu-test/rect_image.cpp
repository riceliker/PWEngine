#include "file.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "utils.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

static const std::string vertex_shader_code = R"(
    #version 450

    layout(location = 0) in vec2 in_position;
    layout(location = 1) in vec3 in_color;
    layout(location = 2) in vec2 in_uv;

    layout(set = 0, binding = 0) uniform RotateUniform {
        mat4 vp;
    } rotate;

    layout(location = 0) out vec3 frag_color;
    layout(location = 1) out vec2 frag_uv;

    void main() {
        gl_Position = rotate.vp * vec4(in_position, 0.0, 1.0);
        frag_color = in_color;
        frag_uv = in_uv;
    }   
)";

static const std::string fragment_shader_code = R"(
    #version 450

    layout(push_constant) uniform ConstData {
        float color_weight;
    } pc; 

    layout(location = 0) in vec3 frag_color;
    layout(location = 1) in vec2 frag_uv;
    layout(location = 0) out vec4 out_color;

    layout(set = 1, binding = 0) uniform sampler2D texture_sampler;

    void main() {
        out_color = mix(texture(texture_sampler, frag_uv), vec4(frag_color, 1), pc.color_weight);
    }
)";

struct MyVertex
{
    PWEngine::Vec2<float> position;
    PWEngine::Vec3<float> color;
    PWEngine::Vec2<float> uv;
};

class MyTask : public PWEngine::GPU::WindowTask
{
private:
    float rotation = 0;
    std::unique_ptr<PWEngine::GPU::RenderPipeline> render_pipeline;

    std::unique_ptr<PWEngine::GPU::GPUVertex> vertex_buffer;

    std::unique_ptr<PWEngine::GPU::BindingLayout> uniform_binding_layout;
    std::unique_ptr<PWEngine::GPU::BindingLayout> sampler_binding_layout;

    std::unique_ptr<PWEngine::GPU::GPUBindingBuffer> uniform_buffer;
    std::unique_ptr<PWEngine::GPU::Binding> uniform_binding;

    std::unique_ptr<PWEngine::GPU::GPUImage> image;
    std::unique_ptr<PWEngine::GPU::GPUImageView> image_view;
    std::unique_ptr<PWEngine::GPU::Binding> image_binding;

    uint8_t value = 0;
public:
    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo window_info) : PWEngine::GPU::WindowTask(instance, window_info)
    {
        std::vector<PWEngine::GPU::BindingInfo> uniform_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
        };
        this->uniform_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(instance, uniform_binding_info);

        std::vector<PWEngine::GPU::BindingInfo> sampler_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::image_sampler},
        };
        this->sampler_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(instance, sampler_binding_info);

        PWEngine::GPU::RenderPipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(MyVertex)}},
            {
                {0, 0, offsetof(MyVertex, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
                {0, 1, offsetof(MyVertex, color), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 2, offsetof(MyVertex, uv), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
            },
            {this->uniform_binding_layout.get(), this->sampler_binding_layout.get()},
            {{0, sizeof(float), PWEngine::GPU::ShaderType::Fragment}},
            PWEngine::GPU::RenderPipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::RenderPipelineInfo::CullMode::None,
            std::nullopt,
            std::nullopt,
            PWEngine::GPU::RenderingInfo{},
        };
        this->render_pipeline = std::make_unique<PWEngine::GPU::RenderPipeline>(instance, pipeline_info);
        std::vector<MyVertex> vertex = {
            {{0.5, 0.5}, {1, 1, 1}, {1, 1}}, {{-0.5, -0.5}, {1, 1, 1}, {0, 0}}, {{-0.5, 0.5}, {1, 1, 1}, {0, 1}},
            {{0.5, 0.5}, {1, 1, 1}, {1, 1}}, {{-0.5, -0.5}, {1, 1, 1}, {0, 0}}, {{0.5, -0.5}, {1, 1, 1}, {1, 0}}
        };
        this->vertex_buffer = std::make_unique<PWEngine::GPU::GPUVertex>(instance, PWEngine::GPU::GPUVertexDataType::vertex, vertex);

        this->uniform_binding = this->uniform_binding_layout->createBinding();
        this->uniform_buffer = std::make_unique<PWEngine::GPU::GPUBindingBuffer>(instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(PWEngine::Mat4));
        this->uniform_binding->update(0, {uniform_buffer.get()});

        auto image = std::make_unique<PWEngine::File::ImageRGBA8>(PWEngine::File::TGA{}, instance->log, "./assets/icon.tga");
        this->image = std::make_unique<PWEngine::GPU::GPUImage>(instance, image.get());
        this->image_view = std::make_unique<PWEngine::GPU::GPUImageView>(this->image.get());

        this->image_binding = this->sampler_binding_layout->createBinding();
        this->image_binding->update(0, {this->image_view.get()});
    } 
    bool loop(double delta) override
    {  
        rotation += delta;
        float c = cos(rotation);
        float s = sin(rotation);
        PWEngine::Mat4 mat({
            c, -s, 0.0f, 0.0f,
            s,  c, 0.0f, 0.0f,
            0.0f,  0.0f, 1.0f, 0.0f,
            0.0f,  0.0f, 0.0f, 1.0f
        });
        
        this->uniform_buffer->writeData(&mat, sizeof(mat));
        this->recordCommand([this, delta](PWEngine::GPU::RecordCommand* cmd){
            cmd->setScissor();
            cmd->setViewPort();
            cmd->bindPipeline(this->render_pipeline.get());
            cmd->bindBinding(this->render_pipeline.get(), {this->uniform_binding.get(), this->image_binding.get()});
            value = (value + 1) % 100;
            float weight = value / 100.0; 
            cmd->pushConstant(this->render_pipeline.get(), 0, sizeof(float), &weight, PWEngine::GPU::ShaderType::Fragment);

            cmd->Rendering({0, 0, 0, 1},true, [this, delta](PWEngine::GPU::RenderingCommand* rendering){
                rendering->bindVertices({this->vertex_buffer.get()}, {0});
                rendering->draw(6, 1, 0, 0);
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