#include "file.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "pipeline.hpp"
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

struct MyVertex
{
    PWEngine::Vec2<float> position;
    PWEngine::Vec3<float> color;
};

class MyTask : public PWEngine::GPU::GPUTaskWithWindow
{
private:
    std::unique_ptr<PWEngine::Pipeline::RectPipeline> rect_pipeline;

    std::unique_ptr<PWEngine::GPU::GPUVertex> vertex_buffer;
    std::unique_ptr<PWEngine::GPU::GPUVertex> index_buffer;
public:
    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo window_info) : PWEngine::GPU::GPUTaskWithWindow(instance, window_info)
    {
        auto icon_image = PWEngine::File::tgaReader(instance->log, "./assets/crosshair.tga");

        this->rect_pipeline = std::make_unique<PWEngine::Pipeline::RectPipeline>(instance);

        // this->vertex_buffer = std::move(PWEngine::GPU::GPUVertex::builderUnique(instance, PWEngine::GPU::GPUVertexDataType::vertex, model->vertices));
        // this->index_buffer = std::move(PWEngine::GPU::GPUVertex::builderUnique(instance, PWEngine::GPU::GPUVertexDataType::index, model->indices));
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