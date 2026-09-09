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

    auto descriptor_set_layout = context->addDescriptorSetLayout();
    auto ubo = context->createUBO(descriptor_set_layout);
    
    const std::vector<PWEngine::Utils::Vertex2D> vertices = {
    {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };
    auto mesh = context->createMesh2D(vertices, indices);

    auto pipeline = context->createPipeline(render_pass, descriptor_set_layout);

    float time = 0;
    while (!context->getIsClosed()) 
    {
        static auto startTime = std::chrono::high_resolution_clock::now();
        glfwPollEvents();
        context->drawFrameCommand(pipeline.get(), [&](PWEngine::Render::FrameSubmitCommand& cmd){
            cmd.setViewPort();
            cmd.setScissor();
            cmd.updateUBO(ubo.get(), time);
            cmd.addMesh2D(mesh.get());
        });
        auto currentTime = std::chrono::high_resolution_clock::now();
        time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    }
    context->waitIdle();

}
