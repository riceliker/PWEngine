#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstdint>

int main()
{
    auto sys = PWEngine::Stream::LogSystem(true);

    PWEngine::Render::InstanceInfo instance_info{};
    instance_info.is_debug = true;
    instance_info.name = "Test";
    instance_info.version = PWEngine::Utils::Vec3<uint8_t>(1, 0, 0);
    PWEngine::Render::Instance instance =
        PWEngine::Render::Instance(instance_info, &sys);

    PWEngine::Render::WindowInfo window_info{};
    window_info.is_resizable = false;
    window_info.title = "Test";
    window_info.size = PWEngine::Utils::Vec2<uint32_t>(1280, 720);

    auto device = instance.GetBestDevice();
    auto window = device->createWindow(window_info);
    auto render_pass = device->createRenderPass();
    auto pipeline = render_pass->createPipeline();
    auto swapchain = window->createSwapchain(render_pass);
    auto cmd_pool = device->createCommandPool();
    auto cmd_buf = cmd_pool->createBuffer();
    auto sync = device->createSync();

    while (!window->isClosed()) 
    {
        glfwPollEvents();
        sync->wait(device);
        swapchain->submit(pipeline, cmd_buf, sync);
    }
    device->waitIdle();

}
