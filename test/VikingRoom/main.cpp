#include "core/render.hpp"
#include "core/stream.hpp"
#include "core/file.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

int main()
{
    auto log = PWEngine::LogSystem(true);

    auto model_image = PWEngine::File::tgaReader(&log, "./assets/viking_room.tga");
    auto icon_image = PWEngine::File::tgaReader(&log, "./assets/crosshair.tga");
    auto model = PWEngine::File::objReader(&log, "./assets/viking_room.obj");

    auto instance = PWEngine::Render::RenderInstance(&log);
    instance.setApplicationName("Test");
    instance.setApplicationVersion(PWEngine::Vec3<uint32_t>(1, 0, 0));
    instance.build();

    PWEngine::Render::ContextInfo context_info{};
    context_info.is_window_resizable = true;
    context_info.window_title = "Test";
    context_info.window_default_resolution = PWEngine::Vec2<uint32_t>(1280, 720);

    auto context = instance.createContext(context_info);
    auto model_texture = context->createTexture2D(model_image.get());
    auto icon_texture = context->createTexture2D(icon_image.get());
    
    auto pipeline3D = context->createPipeline3D({}, {"./shaders/pipeline3D/vert.spv", "./shaders/pipeline3D/frag.spv"});
    auto mesh = pipeline3D->createMesh3D(model.get());
    auto mesh_transform = context->createTransform3D();
    mesh_transform->resizeTransform(2);
    
    mesh->bindTransform3D(mesh_transform);
    
    auto material = pipeline3D->createMaterial();
    material->bindBasicTexture(model_texture);
    material->updateDescriptor();
    auto camera = pipeline3D->createCamera();
    camera->updateDescriptor();

    auto pipeline_hud = context->createPipelineHUD();
    auto set = pipeline_hud->createDS();
    
    set->update(pipeline_hud.get(), icon_texture.get());
    
    size_t loops = 0;
    double total = 0;
    float time = 0;
    float speed = 0.02;
    bool is_in_screen = false;
    PWEngine::Vec2<float> last_mouse = {0, 0};
    PWEngine::Vec2<float> look_degree = {0, 0};
    context->frameLoop([&](PWEngine::Render::Command* cmd, PWEngine::Render::Input* input, float delta){

        if (input->checkIsHoverScreen() && input->checkIsMouseInput(PWEngine::Render::MouseKey::BtnLeft))
        {
            context->hideMouseCursor();
            is_in_screen = true;
            last_mouse = input->checkMousePosition();
        }
        if (input->checkIsKeyInput(PWEngine::Render::Key::Escape))
        {
            context->showMouseCursor();
            is_in_screen = false;
        }
        
        auto current_mouse = last_mouse;
        
        PWEngine::Vec2<float> dm{0,0};
        if (is_in_screen)
        {
            current_mouse = input->checkMousePosition();
            dm = current_mouse - last_mouse;
            last_mouse = current_mouse;
        }

        look_degree = {
            std::clamp(look_degree.x - (float)(dm.y*0.1), -89.0f , 89.0f),
            look_degree.y - (float)(dm.x*0.1)
        };

        last_mouse = current_mouse;

        camera->calculateLookVector(look_degree);

        float speed = 0.04;
        if (input->checkIsKeyInput(PWEngine::Render::Key::W))
        {
            camera->cameraMoveFromLook({speed, 0});
        } 
        if (input->checkIsKeyInput(PWEngine::Render::Key::S)) 
        {
            camera->cameraMoveFromLook({-speed, 0});
        }
        if (input->checkIsKeyInput(PWEngine::Render::Key::A))
        {
            camera->cameraMoveFromLook({0, -speed});
        }
        if (input->checkIsKeyInput(PWEngine::Render::Key::D)) 
        {
            camera->cameraMoveFromLook({0, speed});
        }  

        cmd->setViewPort();
        cmd->setScissor();
        cmd->setPipeline(pipeline3D.get());

        total += delta;
        //PWEngine::sendLog(instance.log, PWEngine::LogType::Debug, PWEngine::LogFrom::Debug, std::to_string(1/delta));
        loops++;

        camera->updateData(cmd);
        mesh_transform->setPosition(0, {0, 0, 0});
        mesh_transform->setRotation(0, {0, 0, 0.5});
        mesh_transform->setPosition(1, {0, 1.5, 0});
        mesh_transform->setScale(1, {1, 1, 2});
        mesh_transform->calculateNodeMatrix(cmd);

        cmd->renderingBegin({0.52, 0.62, 0.65, 1}, true);
        cmd->draw(pipeline3D.get(), mesh.get(), material.get(), camera.get());
        cmd->renderingEnd();

        cmd->renderingBarrier();
        cmd->setViewPort();
        cmd->setScissor();

        cmd->setPipeline(pipeline_hud.get());

        cmd->renderingBegin({0, 0, 0, 0}, false);
        cmd->draw(pipeline_hud.get(), {{-0.1, -0.1, 0.1, 0.1}, {0, 0, 1, 1}, PWEngine::Mat4(1)}, icon_texture.get(), set.get());
        cmd->renderingEnd();

    });

}