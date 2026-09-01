#include "render.hpp"
#include "utils.hpp"

namespace PWEngine::Render::GPU
{
    Window::Window(WindowInfo info)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,
                       info.is_resizable ? GLFW_TRUE : GLFW_FALSE);
        this->ptr = glfwCreateWindow(
            info.size.x, info.size.y, info.title.c_str(), nullptr, nullptr);
    }

    void Window::bindInstance(Instance instance)
    {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance.instance, this->ptr, nullptr, &surface) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
        else 
        {
            this->surface = surface;
        }
    }

    bool Window::isClosed()
    {
        return glfwWindowShouldClose(this->ptr);
    }
} // namespace PWEngine::Render::GPU