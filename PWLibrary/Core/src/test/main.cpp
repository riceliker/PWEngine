#include "core/application.hpp"
#include "core/collections.hpp"
#include <cstdint>
#include<optional>
#include<memory>

using namespace PWL::Core;
int main()
{
    PWL::Core::Init();
    std::optional<std::shared_ptr<Application>> app = Application::CreateApplication(
        "untitled",
        PWL_Vec2<uint32_t>(1280, 720), 
        PWL_Vec2<uint32_t>(1280, 720),
        ApplicationWindowMode::PWL_WINDOWMODE_RESIZABLE
    );
}