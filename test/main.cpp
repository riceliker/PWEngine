#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include "file.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

int main()
{
    auto log = PWEngine::Stream::LogSystem(true);
    log.send(PWEngine::Stream::LogType::Debug, PWEngine::Stream::LogFrom::Debug, "hello world");

    auto model = PWEngine::File::objReader(&log, "./assets/viking_room.obj");
    auto image = PWEngine::File::tgaReader(&log, "./assets/viking_room.tga");

    auto instance = PWEngine::Render::RenderInstance(&log);
    instance.setApplicationName("test");
    instance.setApplicationVersion(PWEngine::Utils::Vec3<uint32_t>(1, 0 , 0));
    instance.build();

    PWEngine::Render::ContextInfo context_info{};
    context_info.is_window_resizable = false;
    context_info.window_title = "Test";
    context_info.window_default_resolution = PWEngine::Utils::Vec2<uint32_t>(1280, 720);
    auto context = instance.createContext(context_info);

    auto texture = context->createTexture2D(image.get());
    
    auto pipeline = context->createPipeline3D({}, {"./shaders/vert.spv", "./shaders/frag.spv"});
    auto mesh = pipeline->createMesh3D(model.get());
    auto material = pipeline->createMaterial();
    material->bindBasicTexture(texture);

    auto camera = pipeline->createCamera();
    camera->calculateLookVector({0, 180});
    camera->position = {3, 0, 0};

    bool is_in_screen = false;
    context->frameLoop([&](PWEngine::Render::Command* cmd, PWEngine::Render::Input* input, float delta){

        if (input->checkIsHoverScreen() && input->checkIsMouseInput(PWEngine::Render::MouseKey::BtnLeft))
        {
            context->hideMouseCursor();
            is_in_screen = true;
        }
        if (input->checkIsKeyInput(PWEngine::Render::Key::Escape))
        {
            context->showMouseCursor();
            is_in_screen = false;
        }

        cmd->setViewPort();
        cmd->setScissor();
        cmd->setPipeline(pipeline.get());

        mesh->addRotation({0, 0, (float)delta});
        mesh->calculateNodeMatrix();

        mesh->update(cmd->swapchain_loop_frame_index);
        material->update(cmd->swapchain_loop_frame_index);
        camera->update(cmd->swapchain_loop_frame_index);

        cmd->renderingBegin({0, 0, 0, 1});
        cmd->draw(cmd->swapchain_loop_frame_index, pipeline.get(), mesh.get(), material.get(), camera.get());
        cmd->renderingEnd();
    });

    // auto log = PWEngine::Stream::LogSystem(true);

    // auto image = PWEngine::File::tgaReader(&log, "./assets/viking_room.tga");
    // auto model = PWEngine::File::objReader(&log, "./assets/viking_room.obj");


    // auto instance = PWEngine::Render::RenderInstance(&log);
    // instance.setApplicationName("Test");
    // instance.setApplicationVersion(PWEngine::Utils::Vec3<uint32_t>(1, 0, 0));
    // instance.build();

    // PWEngine::Render::ContextInfo context_info{};
    // context_info.is_window_resizable = false;
    // context_info.window_title = "Test";
    // context_info.window_default_resolution = PWEngine::Utils::Vec2<uint32_t>(1280, 720);

    // auto context = instance.createContext(context_info);
    // auto texture = context->createTexture2D(image.get());
    
    // auto pipeline = context->createPipeline3D({}, {"./shaders/vert.spv", "./shaders/frag.spv"});
    // auto mesh = pipeline->createMesh3D(model.get());
    
    // auto material = pipeline->createMaterial();
    // material->bindBasicTexture(texture);
    // auto camera = pipeline->createCamera();
    
    // size_t loops = 0;
    // double total = 0;
    // float time = 0;
    // float speed = 0.02;
    // bool is_in_screen = false;
    // PWEngine::Utils::Vec2<float> last_mouse = {0, 0};
    // PWEngine::Utils::Vec2<float> look_degree = {0, 0};
    // context->frameLoop([&](PWEngine::Render::Command* cmd, PWEngine::Render::Input* input, float delta){

    //     if (input->checkIsHoverScreen() && input->checkIsMouseInput(PWEngine::Render::MouseKey::BtnLeft))
    //     {
    //         context->hideMouseCursor();
    //         is_in_screen = true;
    //     }
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::Escape))
    //     {
    //         context->showMouseCursor();
    //         is_in_screen = false;
    //     }
        
    //     auto current_mouse = last_mouse;
        
    //     if (is_in_screen)
    //         current_mouse = input->checkMousePosition();
            
    //     auto dm = current_mouse - last_mouse;
    //     //PWEngine::Stream::log(instance.log, PWEngine::Stream::LogType::Debug, PWEngine::Stream::LogFrom::Debug, std::to_string(dm.x) + "," + std::to_string(dm.y));
        
    //     look_degree = {
    //         std::clamp(look_degree.x - (float)(dm.y*0.1), -89.0f , 89.0f),
    //         look_degree.y - (float)(dm.x*0.1)
    //     };

    //     //PWEngine::Stream::log(instance.log, PWEngine::Stream::LogType::Debug, PWEngine::Stream::LogFrom::Debug, std::to_string(look_degree.x) + "," + std::to_string(look_degree.y));

    //     last_mouse = current_mouse;

    //     camera->calculateLookVector(look_degree);

    //     float speed = 0.04;
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::W))
    //     {
    //         camera->cameraMoveFromLook({speed, 0});
    //     } 
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::S)) 
    //     {
    //         camera->cameraMoveFromLook({-speed, 0});
    //     }
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::A))
    //     {
    //         camera->cameraMoveFromLook({0, -speed});
    //     }
    //     if (input->checkIsKeyInput(PWEngine::Render::Key::D)) 
    //     {
    //         camera->cameraMoveFromLook({0, speed});
    //     }  

    //     //PWEngine::Stream::log(instance.log, PWEngine::Stream::LogType::Debug, PWEngine::Stream::LogFrom::Debug, std::to_string(camera->position.x) + "," + std::to_string(camera->position.y) + "," + std::to_string(camera->position.z));

    //     cmd->setViewPort();
    //     cmd->setScissor();
    //     cmd->setPipeline(pipeline.get());

    //     total += 1/delta;
    //     PWEngine::Stream::log(instance.log, PWEngine::Stream::LogType::Debug, PWEngine::Stream::LogFrom::Debug, std::to_string(1/delta));
    //     loops++;

    //     mesh->update(cmd->swapchain_loop_frame_index);
    //     material->update(cmd->swapchain_loop_frame_index);
    //     camera->update(cmd->swapchain_loop_frame_index);

    //     cmd->renderingBegin({0, 0, 0, 1});
    //     cmd->draw(cmd->swapchain_loop_frame_index, pipeline.get(), mesh.get(), material.get(), camera.get());
    //     cmd->renderingEnd();

    // });

}