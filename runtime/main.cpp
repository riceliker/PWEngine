#include "PWEngine.hpp"
#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstdint>

int main()
{
    auto sys = PWEngine::Stream::Log::LogSystem(true);
    sys.send(PWEngine::Stream::Log::LogType::Debug,
             PWEngine::Stream::Log::LogFrom::Debug,
             "error");

    PWEngine::Render::GPU::WindowInfo window_info{};
    window_info.is_resizable = false;
    window_info.title = "Test";
    window_info.size = PWEngine::Utils::Vec2<uint32_t>(1280, 720);
    PWEngine::Render::GPU::Window window = PWEngine::Render::GPU::Window(window_info);

    PWEngine::Render::GPU::InstanceInfo instance_info{};
    instance_info.is_debug = true;
    instance_info.name = "Test";
    instance_info.version = PWEngine::Utils::Vec3<uint8_t>(1, 0, 0);
    PWEngine::Render::GPU::Instance instance =
        PWEngine::Render::GPU::Instance(instance_info, window);

    window.bindInstance(instance);

    auto device = instance.GetBestDevice(window);
    auto swapchain = PWEngine::Render::GPU::Swapchain(device.value(), window);
    auto pipeline = PWEngine::Render::GPU::Pipeline(device.value(), swapchain);
    swapchain.bindPipeline(device.value(), pipeline);
    auto cmd_buf = PWEngine::Render::GPU::CommandBuffer(device.value(), window);
    auto sync = PWEngine::Render::GPU::Sync(device.value());

    while (!window.isClosed()) 
    {
        glfwPollEvents();
        sync.wait(device.value());
        swapchain.submit(device.value(), pipeline, cmd_buf, sync);

    }
    device->waitIdle();

}
