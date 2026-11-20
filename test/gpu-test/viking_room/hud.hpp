#include "file/tga.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

static const std::string hud_vertex_shader_code = R"(
    #version 450

    layout(location = 0) in vec2 in_position;
    layout(location = 1) in vec2 in_uv;

    layout(set = 0, binding = 0) uniform RotateUniform {
        mat4 vp;
    } rotate;

    layout(location = 0) out vec2 frag_uv;

    void main() {
        gl_Position = rotate.vp * vec4(in_position, 0.0, 1.0);
        frag_uv = in_uv;
    }   
)";

static const std::string hud_fragment_shader_code = R"(
    #version 450

    layout(location = 0) in vec2 frag_uv;
    layout(location = 0) out vec4 out_color;

    layout(set = 1, binding = 0) uniform sampler2D texture_sampler;

    void main() {
        out_color = texture(texture_sampler, frag_uv);
    }
)";

struct MyVertex
{
    PWEngine::Vec2<float> position;
    PWEngine::Vec2<float> uv;
};

struct HUDPipeline
{
    PWEngine::GPU::GPUInstance* p_instance;

    std::unique_ptr<PWEngine::GPU::RenderPipeline> pipeline;
    std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
    std::unique_ptr<PWEngine::GPU::Binding> binding;

    std::unique_ptr<PWEngine::GPU::GPULocalBuffer> vertex_buffer;
    std::unique_ptr<PWEngine::GPU::GPUBindingBuffer> uniform_buffer;
    std::unique_ptr<PWEngine::GPU::GPUImage> image;
    std::unique_ptr<PWEngine::GPU::GPUImageView> image_view;

    HUDPipeline(PWEngine::GPU::GPUInstance* instance)
    {
        std::vector<PWEngine::GPU::BindingInfo> uniform_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
            {1, 1, PWEngine::GPU::ShaderType::Fragment, PWEngine::GPU::BindingType::image_sampler},
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(instance, uniform_binding_info);

        PWEngine::GPU::RenderPipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(hud_vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(hud_fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(MyVertex)}},
            {
                {0, 0, offsetof(MyVertex, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
                {0, 1, offsetof(MyVertex, uv), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
            },
            {this->layout.get()},
            {{0, sizeof(float), PWEngine::GPU::ShaderType::Fragment}},
            PWEngine::GPU::RenderPipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::RenderPipelineInfo::CullMode::None,
            std::nullopt,
            std::nullopt,
        };
        this->pipeline = std::make_unique<PWEngine::GPU::RenderPipeline>(instance, pipeline_info);
        std::vector<MyVertex> vertex = {
            {{0.1, 0.1}, {1, 1}}, {{-0.1, -0.1}, {0, 0}}, {{-0.1, 0.1}, {0, 1}},
            {{0.1, 0.1}, {1, 1}}, {{-0.1, -0.1}, {0, 0}}, {{0.1, -0.1}, {1, 0}}
        };
        this->vertex_buffer = std::make_unique<PWEngine::GPU::GPULocalBuffer>(instance, PWEngine::GPU::BufferUsage::vertex, sizeof(MyVertex) * vertex.size(), vertex.data());

        this->binding = this->layout->createBinding();
        this->uniform_buffer = std::make_unique<PWEngine::GPU::GPUBindingBuffer>(instance, PWEngine::GPU::BufferUsage::uniform, sizeof(PWEngine::Mat4));
        
        auto [image, image_err] = PWEngine::Tga::TgaFile("./assets/crosshair.tga");
        PWEngine::sendLog(this->p_instance->log, PWEngine::LogType::Debug, PWEngine::LogFrom::Debug, std::to_string(static_cast<uint8_t>(image_err)));
        this->image = std::make_unique<PWEngine::GPU::GPUImage>(instance, image.get(), true, true);
        this->image_view = std::make_unique<PWEngine::GPU::GPUImageView>(this->image.get());

        this->binding->update(0, {this->uniform_buffer.get(), this->image_view.get()});
    }
};