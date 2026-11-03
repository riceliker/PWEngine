#include "gpu.hpp"
#include "math.hpp"
#include "stream.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

static const std::string vertex_shader_code = R"(
    #version 450

    layout(location = 0) in vec2 in_position;
    layout(location = 1) in vec3 in_color;

    layout(set = 0, binding = 0) uniform RotateUniform {
        mat4 vp;
    } rotate;

    layout(location = 0) out vec3 frag_color;

    void main() {
        gl_Position = rotate.vp * vec4(in_position, 0.0, 1.0);
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

struct MyVertex
{
    PWEngine::Vec2<float> position;
    PWEngine::Vec3<float> color;
    PWEngine::Vec2<float> uv;
};

class MyTask : public PWEngine::GPU::GPUTaskWithWindow
{
private:
    float rotation = 0;
    std::unique_ptr<PWEngine::GPU::Pipeline> render_pipeline;

    std::unique_ptr<PWEngine::GPU::GPUVertex> vertex_buffer;
    std::unique_ptr<PWEngine::GPU::GPUVertex> index_buffer;
    std::unique_ptr<PWEngine::GPU::BindingLayout> uniform_binding_layout;
    std::unique_ptr<PWEngine::GPU::Binding> uniform_binding0;
    std::unique_ptr<PWEngine::GPU::GPUBinding> uniform_buffer0;
    std::unique_ptr<PWEngine::GPU::BindingLayout> sampler_binding;
public:
    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo window_info) : PWEngine::GPU::GPUTaskWithWindow(instance, window_info)
    {
        std::vector<PWEngine::GPU::BindingInfo> uniform_binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Vertex, PWEngine::GPU::BindingType::uniform},
        };
        this->uniform_binding_layout = std::make_unique<PWEngine::GPU::BindingLayout>(instance, uniform_binding_info);

        PWEngine::GPU::PipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(MyVertex)}},
            {
                {0, 0, offsetof(MyVertex, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
                {0, 1, offsetof(MyVertex, color), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
            },
            {this->uniform_binding_layout.get()},
            {},
            PWEngine::GPU::PipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::PipelineInfo::CullMode::None,
            std::nullopt,
            std::nullopt,
            PWEngine::GPU::RenderingInfo{},
        };
        this->render_pipeline = std::make_unique<PWEngine::GPU::Pipeline>(instance, pipeline_info);
        std::vector<MyVertex> vertex = {
            {{0.5, 0.5}, {1, 1, 1}, {1, 1}}, {{-0.5, -0.5}, {1, 1, 1}, {-1, -1}}, {{-0.5, 0.5}, {1, 1, 1}, {-1, 1}},
            {{0.5, 0.5}, {1, 1, 1}, {1, 1}}, {{-0.5, -0.5}, {1, 1, 1}, {-1, -1}}, {{0.5, -0.5}, {1, 1, 1}, {1, -1}}
        };
        this->vertex_buffer = std::move(PWEngine::GPU::GPUVertex::builderUnique(instance, PWEngine::GPU::GPUVertexDataType::vertex, vertex));
        std::vector<uint32_t> index = {1 , 2, 3, 4, 5, 6};
        this->index_buffer = std::move(PWEngine::GPU::GPUVertex::builderUnique(instance, PWEngine::GPU::GPUVertexDataType::index, index));

        this->uniform_binding0 = this->uniform_binding_layout->createBinding();
        this->uniform_buffer0 = std::make_unique<PWEngine::GPU::GPUBinding>(instance, PWEngine::GPU::GPUBindingDataType::uniform, sizeof(PWEngine::Mat4));
        this->uniform_binding0->update(0, {std::tuple<PWEngine::GPU::GPUBinding*, PWEngine::Vec2<uint32_t>>{uniform_buffer0.get(), {0, 0}}});
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
        
        this->uniform_buffer0->writeData(&mat, sizeof(mat));
        this->recordCommand([this](PWEngine::GPU::RecordCommandWithWindow* cmd){
            cmd->setScissor();
            cmd->setViewPort();
            cmd->bindPipeline(this->render_pipeline.get(), PWEngine::GPU::PipelineType::Render);

            cmd->Rendering({0, 0, 0, 1},true, [this](PWEngine::GPU::RenderingCommand& rendering){
                rendering.bindBinding({this->uniform_binding0.get()}, this->render_pipeline.get());
                rendering.bindVertices({this->vertex_buffer.get()}, {0});
                rendering.bindIndex(this->index_buffer.get(), PWEngine::GPU::InputUintType::u32);
                rendering.draw(6, 1, 0, 0);
            });
        });
        return false;
    }
};


int main()
{
    PWEngine::LogSystem log(true);

    PWEngine::GPU::GPUInstanceInfo instance_info = {
        true, "Viking Room", {1, 0, 0}
    };
    PWEngine::GPU::GPUInstance instance(&log, instance_info);

    PWEngine::GPU::WindowInfo window_info = {
        {1280, 720}, 
        "Triangle",
        true
    };
    
    auto main_task_id = instance.createGPUTask<MyTask>(&instance, window_info);

    instance.run(main_task_id);
}