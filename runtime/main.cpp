#include "PWEngine.hpp"
#include "Render.hpp"
#include "Utils.hpp"
#include <cstdint>

int main()
{
    PWEngine::Render::WindowInfo window_info{};
    window_info.is_resizable = false;
    window_info.title = "Test";
    window_info.size = PWEngine::Utils::Vec2<uint32_t>(1280, 720);
    PWEngine::Render::Window window = PWEngine::Render::Window(window_info);

    PWEngine::Render::GPU::InstanceInfo instance_info{};
    instance_info.is_debug = true;
    instance_info.name = "Test";
    instance_info.version = PWEngine::Utils::Vec3<uint8_t>(1, 0, 0);
    PWEngine::Render::GPU::Instance instance = PWEngine::Render::GPU::Instance(instance_info);

    

}
