#include "file.hpp"
#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <print>

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

    
    auto pipeline = context->createPipeline3D({}, {"./shaders/vert.spv", "./shaders/frag.spv"});
    auto mesh = pipeline->createMesh3D(model.get(), image.get());
    auto camera = pipeline->creatCamera();
    
    float time = 0;
    float speed = 0.02;
    bool is_in_screen = false;
    PWEngine::Utils::Vec2<int> last_mouse = {0, 0};
    PWEngine::Utils::Vec2<float> look_degree;
    context->frameLoop([&](){
        context->__waitFence();

        context->checkMouse(is_in_screen);
        auto currect_mouse = last_mouse;
        
        if (is_in_screen)
            currect_mouse = context->getMouse();
            
        auto dm = currect_mouse - last_mouse;
        
        look_degree = {
            std::clamp(look_degree.x - (float)(dm.y*0.1), -89.0f , 89.0f),
            look_degree.y - (float)(dm.x*0.1)
        };

        last_mouse = currect_mouse;

        camera->calculateLookVector(look_degree);

        float speed = 0.04;
        if (context->checkIsInput(GLFW_KEY_W))
        {
            camera->cameraMoveFromLook({speed, 0});
        } 
        if (context->checkIsInput(GLFW_KEY_S)) 
        {
            camera->cameraMoveFromLook({-speed, 0});
        }
        if (context->checkIsInput(GLFW_KEY_A))
        {
            camera->cameraMoveFromLook({0, -speed});
        }
        if (context->checkIsInput(GLFW_KEY_D)) 
        {
            camera->cameraMoveFromLook({0, speed});
        }  

        time += context->delta;
        mesh->data.model = PWEngine::Utils::translate(PWEngine::Utils::Vec3<float>(0, 0, 0)) * PWEngine::Utils::rotate(PWEngine::Utils::Mat4(1.0), 0 * PWEngine::Utils::deg2rad(90), PWEngine::Utils::Vec3<float>(0, 0, 1));
        mesh->update(context->current_frame);
        camera->update(context->current_frame);

        auto cmd = context->__frameCommandStart();
        auto rendering = cmd->__frameRenderingStart();

        rendering->setViewPort();
        rendering->setScissor();
        rendering->setPipeline(pipeline.get());

        rendering->setDescriptorSet(pipeline.get());

        camera->setData(context->current_frame);
        mesh->setData();


        mesh->bind(rendering);

        cmd->__frameRenderingEnd(rendering);

        context->__frameCommandEnd(cmd);
        context->frameSubmit();
    });

}
