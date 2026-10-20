#include "core/render.hpp"
#include "core/stream.hpp"

int main(int argc, char** argv)
{
    auto log = PWEngine::LogSystem(true);

    auto instance = PWEngine::Render::RenderInstance(&log);
    instance.setApplicationName("Test");
    instance.setApplicationVersion(PWEngine::Vec3<uint32_t>(1, 0, 0));
    instance.build();

    PWEngine::Render::ContextInfo context_info{};
    context_info.is_window_resizable = true;
    context_info.window_title = "Test";
    context_info.window_default_resolution = PWEngine::Vec2<uint32_t>(1280, 720);

    auto context = instance.createContext(context_info);

    auto pipeline = context->createPipeline2D();

    context->frameLoop([&](PWEngine::Render::Command* cmd, PWEngine::Render::Input* input, float delta){
        cmd->setViewPort();
        cmd->setScissor();
        cmd->setPipeline2D(pipeline.get());

        cmd->renderingBegin({0, 0, 0, 1});
        
        cmd->renderingEnd();
    });
}