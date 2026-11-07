#include "gpu.hpp"
#include "math.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <optional>
#include <vector>

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

static const std::string compute_shader_code = R"(
    #version 450
    layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
    layout(push_constant) uniform ConstData {
        float data;
    } pc; 
    
    layout (set = 0, binding = 0) buffer StorageBuffer {
        float data[];
    } ssbo;

    void main() {
        uint index = gl_GlobalInvocationID.x;
        for (int i = 0; i < 256; ++i) {
            ssbo.data[index + 256 * i] = pc.data; 
        }
    }   
)";

struct MyVertex
{
    PWEngine::Vec2<float> position;
    PWEngine::Vec3<float> color;
};

class MyTask : public PWEngine::GPU::WindowTask
{
private:
    PWEngine::GPU::GPUInstance* instance;
    std::unique_ptr<PWEngine::GPU::RenderPipeline> render_pipeline;

    std::unique_ptr<PWEngine::GPU::ComputePipeline> compute_pipeline;
    std::unique_ptr<PWEngine::GPU::BindingLayout> layout;
    std::unique_ptr<PWEngine::GPU::Binding> binding;
    std::unique_ptr<PWEngine::GPU::GPUBindingBuffer> binding_buffer;

    std::unique_ptr<PWEngine::GPU::GPULocalBuffer> vertex_buffer;
public:
    void createRender() 
    {
        PWEngine::GPU::RenderPipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(MyVertex)}},
            {
                {0, 0, offsetof(MyVertex, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
                {0, 1, offsetof(MyVertex, color), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
            },
            {},
            {},
            PWEngine::GPU::RenderPipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::RenderPipelineInfo::CullMode::None,
            std::nullopt,
            std::nullopt,
            PWEngine::GPU::RenderingInfo{},
        };
        this->render_pipeline = std::make_unique<PWEngine::GPU::RenderPipeline>(instance, pipeline_info);
        std::vector<MyVertex> vertex = {{{0, -0.25}, {1, 0, 0}}, {{-0.5, 0.25}, {0, 1, 0}}, {{0.5, 0.25}, {0, 0, 1}}};
        this->vertex_buffer = std::make_unique<PWEngine::GPU::GPULocalBuffer>(instance, PWEngine::GPU::BufferUsage::vertex, vertex.size() * sizeof(MyVertex), vertex.data());
    }
    void createCompute()
    {
        std::vector<PWEngine::GPU::BindingInfo> binding_info = {
            {0, 1, PWEngine::GPU::ShaderType::Compute, PWEngine::GPU::BindingType::storage}
        };
        this->layout = std::make_unique<PWEngine::GPU::BindingLayout>(instance, binding_info);

        PWEngine::GPU::ComputePipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(compute_shader_code, PWEngine::GPU::ShaderType::Compute, "main").value(),
            {this->layout.get()},
            {
                {0, 4, PWEngine::GPU::ShaderType::Compute}
            },
        };
        this->compute_pipeline = std::make_unique<PWEngine::GPU::ComputePipeline>(instance, pipeline_info);

        float* data = (float*) malloc(4 * 65536);
        this->binding_buffer = std::make_unique<PWEngine::GPU::GPUBindingBuffer>(instance, PWEngine::GPU::BufferUsage::storage, 65536 * sizeof(float), data);
        this->binding = this->layout->createBinding();
        this->binding->update(0, {this->binding_buffer.get()});
    }
    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo window_info) : PWEngine::GPU::WindowTask(instance, window_info)
    {
        this->instance = instance;
        this->createRender();
        this->createCompute();
    }
    bool process(double delta) override
    {
        PWEngine::sendLog(this->p_instance->log, PWEngine::LogType::Debug, PWEngine::LogFrom::Debug, "The data in 1:" + std::to_string(((float*)this->binding_buffer->map_pointer)[0]));
        this->recordCommand([this](PWEngine::GPU::RecordCommand* cmd){
            cmd->setScissor({0, 0}, this->m_window->getWindowSize());
            cmd->setViewPort({0, 0}, this->m_window->getWindowSize(), {0, 1});
            cmd->bindPipeline(this->render_pipeline.get());

            cmd->Rendering({0, 0, 0, 1}, true, this, [this](PWEngine::GPU::RenderingCommand* rendering){
                rendering->bindVertices({this->vertex_buffer.get()}, {0});
                rendering->draw(3, 1, 0, 0);
            });

            cmd->bindPipeline(this->compute_pipeline.get());
            cmd->bindBinding(this->compute_pipeline.get(), {this->binding.get()});
            float a = 12;
            cmd->pushConstant(this->compute_pipeline.get(), 0, sizeof(float), &a, PWEngine::GPU::ShaderType::Compute);
            cmd->Dispatch({256, 1, 1});

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
    
    auto main_task_id = instance.createWindowTask<MyTask>(&instance, window_info);

    instance.run(main_task_id);
}