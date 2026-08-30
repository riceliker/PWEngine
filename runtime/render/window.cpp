#include "Render.hpp"
#include "Utils.hpp"

namespace PWEngine::Render
{
    Window::Window(WindowInfo info)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, info.is_resizable?GLFW_TRUE:GLFW_FALSE);
        this->ptr = glfwCreateWindow(info.size.x, info.size.y, info.title.c_str(), nullptr, nullptr);
    }
}