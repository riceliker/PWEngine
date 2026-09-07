#include "file.hpp"
#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstdint>
#include "file.hpp"

int main()
{
    auto log = PWEngine::Stream::LogSystem(true);

    auto tga = PWEngine::File::TgaStream(&log, "./assets/icon.tga");

    PWEngine::Render::InstanceInfo instance_info{};
    instance_info.is_debug = true;
    instance_info.name = "Test";
    instance_info.version = PWEngine::Utils::Vec3<uint8_t>(1, 0, 0);
    PWEngine::Render::Instance instance =
        PWEngine::Render::Instance(instance_info, &log);

    PWEngine::Render::WindowInfo window_info{};
    window_info.is_resizable = false;
    window_info.title = "Test";
    window_info.size = PWEngine::Utils::Vec2<uint32_t>(1280, 720);

    auto context = instance.createContext(window_info);
    auto render_pass = context->addRenderPass();
    context->createSwapchain(render_pass);
    
    const std::vector<PWEngine::Render::Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    auto vertex_buffer = context->createVertexBuffer(vertices);

    auto pipeline = context->createPipeline(render_pass);

    while (!context->getIsClosed()) 
    {
        glfwPollEvents();
        context->drawFrame(pipeline.get(), [&](PWEngine::Render::FrameSubmitCommand& cmd){
            cmd.setViewPort();
            cmd.setScissor();
            cmd.addVertexBuffer(vertex_buffer.get());
        });
    }
    context->waitIdle();

}
