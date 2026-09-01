#include "PWEngine.hpp"
#include "stream.hpp"
#include "render.hpp"
#include "utils.hpp"
#include <cstdint>

int main()
{
    auto sys = PWEngine::Stream::Log::LogSystem(true);
    sys.send(PWEngine::Stream::Log::LogType::Debug, PWEngine::Stream::Log::LogFrom::Debug, "error");

    PWEngine::Render::WindowInfo window_info{};
    window_info.is_resizable = false;
    window_info.title = "Test";
    window_info.size = PWEngine::Utils::Vec2<uint32_t>(1280, 720);
    PWEngine::Render::Window window = PWEngine::Render::Window(window_info);

    PWEngine::Render::GPU::InstanceInfo instance_info{};
    instance_info.is_debug = true;
    instance_info.name = "Test";
    instance_info.version = PWEngine::Utils::Vec3<uint8_t>(1, 0, 0);
    PWEngine::Render::GPU::Instance instance = PWEngine::Render::GPU::Instance(instance_info, window);

    
    

}
