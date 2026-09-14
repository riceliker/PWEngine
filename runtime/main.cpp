#include "file.hpp"
#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstdint>
#include "file.hpp"

int main()
{
    auto log = PWEngine::Stream::LogSystem(true);

    auto tga = PWEngine::File::TgaStream(&log, "./assets/viking_room.tga");
    auto image = tga.asImage();

    PWEngine::Render::InstanceInfo instance_info = {
        .name = "Test",
        .version = PWEngine::Utils::Vec3<uint8_t>(1, 0, 0)
    };
    instance_info.is_debug = true;
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
    auto descriptor_sets = context->addDescriptorSet(descriptor_set_layout);
    auto ubo = context->createUBO();
    auto texture = context->createTexture2D(image.get());
    context->updateDescriptor(descriptor_sets, ubo.get(), texture.get());
    
    auto obj = PWEngine::File::ObjStream(&log, "./assets/viking_room.obj");
    auto vertices = obj.asVertex3D();
    auto indices = obj.asIndices();
    auto mesh = context->createMesh3D(vertices, indices);

    auto pipeline = context->createPipeline(render_pass, descriptor_set_layout);

    float time = 0;
    while (!context->getIsClosed()) 
    {
        static auto startTime = std::chrono::high_resolution_clock::now();
        glfwPollEvents();
        context->drawFrameCommand(pipeline.get(), descriptor_sets, [&](PWEngine::Render::FrameSubmitCommand& cmd){
            cmd.setViewPort();
            cmd.setScissor();
            PWEngine::Render::CameraInfo camera_info = {
                .camera_pos = PWEngine::Utils::Vec3<float>(0.0f, -5.0f, 0.0f),
                .camera_look_pos = PWEngine::Utils::Vec3<float>(0.0f, 0.0f, 0.0f)
            };
            cmd.updateUBO(ubo.get(), time, camera_info);
            cmd.addMesh3D(mesh.get());
        });
        auto currentTime = std::chrono::high_resolution_clock::now();
        time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    }
    context->waitIdle();

}
