#include "gpu.hpp"
#include "math.hpp"
#include "stream.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

struct MyTasKInfo
{
    std::unique_ptr<PWEngine::GPU::Pipeline> render_pipeline;
};

struct MyVertex
{
    PWEngine::Vec2<float> position;
    PWEngine::Vec3<float> color;
};

class MyTask : public PWEngine::GPU::GPUTaskWithWindow
{
private:
    std::unique_ptr<PWEngine::GPU::Pipeline> render_pipeline;

    std::unique_ptr<PWEngine::GPU::GPUBuffer> vertex_buffer;
    std::unique_ptr<PWEngine::GPU::GPUBuffer> index_buffer;
public:
    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo window_info, PWEngine::GPU::PipelineInfo& pipeline_info) : PWEngine::GPU::GPUTaskWithWindow(instance, window_info)
    {
        this->render_pipeline = std::make_unique<PWEngine::GPU::Pipeline>(instance, pipeline_info);
        std::vector<MyVertex> vertex = {{{0, -0.25}, {1, 0, 0}}, {{-0.5, 0.25}, {0, 1, 0}}, {{0.5, 0.25}, {0, 0, 1}}};
        this->vertex_buffer = std::move(PWEngine::GPU::GPUBuffer::builderUnique(instance, PWEngine::GPU::GPUBufferType::vertex, vertex));
        std::vector<uint32_t> index = {0, 1, 2};
        this->index_buffer = std::move(PWEngine::GPU::GPUBuffer::builderUnique(instance, PWEngine::GPU::GPUBufferType::index, index));
    }
    bool loop(double delta) override
    {
        this->recordCommand([this](PWEngine::GPU::RecordCommandWithWindow* cmd){
            cmd->setScissor();
            cmd->setViewPort();
            cmd->bindPipeline(this->render_pipeline.get(), PWEngine::GPU::PipelineType::Render);

            cmd->Rendering({0, 0, 0, 1},true, [this](PWEngine::GPU::RenderingCommand& rendering){
                rendering.bindVertices({this->vertex_buffer.get()}, {0});
                rendering.bindIndex(this->index_buffer.get(), PWEngine::GPU::InputUintType::u32);
                rendering.draw(3, 1, 0, 0);
            });
        });
        return false;
    }
};



static const std::string vertex_shader_code = R"(
    #version 450

    layout(location = 0) in vec2 in_position;
    layout(location = 1) in vec3 in_color;

    layout(location = 0) out vec3 frag_color;

    void main() {
        gl_Position = vec4(in_position, 0.0, 1.0);
        frag_color = in_color;
    }   
)";

static const std::string fragment_shader_code = R"(
    #version 450

    layout(location = 0) in vec3 frag_color;

    layout(location = 0) out vec4 out_color;

    void main() {
        out_color = vec4(frag_color, 1.0);
    }
)";

int main()
{
    PWEngine::LogSystem log(true);

    PWEngine::GPU::GPUInstanceInfo instance_info = {
        false, "Viking Room", {1, 0, 0}
    };
    PWEngine::GPU::GPUInstance instance(&log, instance_info);

    PWEngine::GPU::WindowInfo window_info = {
        {1280, 720}, 
        "Viking Room",
        true
    };

    PWEngine::GPU::PipelineInfo pipeline_info = {
        PWEngine::GPU::compilerShader(vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
        PWEngine::GPU::compilerShader(fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
        {{0,sizeof(MyVertex)}},
        {
            {0, 0, offsetof(MyVertex, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
            {0, 1, offsetof(MyVertex, color), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
        },
        std::nullopt,
        std::nullopt,
        PWEngine::GPU::PipelineInfo::VertexTopology::Triangle_List,
        PWEngine::GPU::PipelineInfo::CullMode::None,
        std::nullopt,
        {0, 0, 0, 0},
        std::nullopt
    };
    auto main_task_id = instance.createGPUTask<MyTask>(&instance, window_info, pipeline_info);

    instance.run(main_task_id);


}