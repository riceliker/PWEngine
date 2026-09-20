#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstdint>

int main()
{
    auto log = PWEngine::Stream::LogSystem(true);
    log.send(PWEngine::Stream::LogType::Debug, PWEngine::Stream::LogFrom::Debug, "hello world");

    auto instance = PWEngine::Render::RenderInstance(&log);
    instance.setApplicationName("test");
    instance.setApplicationVersion(PWEngine::Utils::Vec3<uint32_t>(1, 0 , 0));
    instance.build();

    PWEngine::Render::ContextInfo context_info{};
    context_info.is_window_resizable = false;
    context_info.window_title = "Test";
    context_info.window_default_resolution = PWEngine::Utils::Vec2<uint32_t>(1280, 720);
    auto context = instance.createContext(context_info);
    
    auto pipeline = context->createPipeline3D({}, {"./shaders/vert.spv", "./shaders/frag.spv"});


    context->frameLoop([&](float delta){

    });
}