# PWEngine
The game engine for game Physic World.

## Platform
No DRM Prison:
1. PWEngine is **never** support for mobile device.  
Such as Android and iOS.  
2. PWEngine is **never** support for Game Terminal.  
Such as Xbox, PlayStation and Switch.  

| platform | Windows | macOS | Linux |
|:---:|:---:|:---:|:---:|
| API | Vulkan | Vulkan(MoltenVK) | Vulkan |
| Arch | amd64,arm64 | arm64 | amd64,arm64 |

## Simple
```C++
int main()
{
    auto log = PWEngine::Stream::LogSystem(true);

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

    auto device = instance.GetBestDevice();
    auto window = device->createWindow(window_info);
    auto render_pass = device->createRenderPass();
    
    const std::vector<PWEngine::Render::Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    auto vertex_buffer = device->createVertexBuffer(vertices);

    auto pipeline = render_pass->createPipeline();
    auto swapchain = window->createSwapchain(render_pass);
    auto cmd_pool = device->createCommandPool();
    auto cmd_buf = cmd_pool->createBuffer();
    auto sync = device->createSync();

    while (!window->isClosed()) 
    {
        glfwPollEvents();
        swapchain->submit(render_pass, cmd_buf, sync, [&](auto cmd){
            cmd.bindPipeline(pipeline);
            cmd.setViewPort(swapchain);
            cmd.setScissor(swapchain);
            cmd.addVertexBuffer(vertex_buffer);
        });
        sync->wait(device);
    }
    device->waitIdle();
}
```