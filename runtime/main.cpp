#include "file.hpp"
#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstdint>
#include <memory>

int main()
{
    auto log = PWEngine::Stream::LogSystem(true);

    auto image = PWEngine::File::TgaStream(&log, "./assets/viking_room.tga").asImage();
    auto model = PWEngine::File::ObjStream(&log, "./assets/viking_room.obj").asModel();


    auto instance = PWEngine::Render::RenderInstance(&log);
    instance.setApplicationName("Test");
    instance.setApplicationVersion(PWEngine::Utils::Vec3<uint32_t>(1, 0, 0));
    instance.build();

    PWEngine::Render::ContextInfo context_info{};
    context_info.is_window_resizable = false;
    context_info.window_title = "Test";
    context_info.window_default_resolution = PWEngine::Utils::Vec2<uint32_t>(1280, 720);

    auto context = instance.createContext(context_info);

    auto model_descriptor_set_layout = context->addModelDescriptorSetLayout();
    auto camera_descriptor_set_layout = context->addCameraDescriptorSetLayout();
    auto mesh = context->createMesh3D(model.get(), image.get(), model_descriptor_set_layout);
    auto camera = context->creatCamera(camera_descriptor_set_layout);
    auto pipeline = context->createPipeline({model_descriptor_set_layout, camera_descriptor_set_layout});
    
    float time = 0;
    context->frameLoop([&](){
        context->__waitFence();

        time += context->delta;
        camera->setLookAt(PWEngine::Utils::Vec3<float>(0, -2, 2), PWEngine::Utils::Vec3<float>(0, 0, 0), 60, 0.01, 100);
        mesh->data.model = PWEngine::Utils::rotate(PWEngine::Utils::Mat4(1.0), time * PWEngine::Utils::deg2rad(90), PWEngine::Utils::Vec3<float>(0, 0, 1));
        camera->update(context->current_frame);
        mesh->update(context->current_frame);
        

        auto cmd = context->__frameCommandStart();
        auto rendering = cmd->__frameRenderingStart();

        rendering->setViewPort();
        rendering->setScissor();
        rendering->setPipeline(pipeline.get());
        rendering->addDescriptorSet(mesh.get());
        rendering->addDescriptorSet(camera.get());
        rendering->bindDescriptorSets();
        mesh->bind(rendering);

        cmd->__frameRenderingEnd(rendering);

        context->__frameCommandEnd(cmd);
        context->frameSubmit();
    });

}
