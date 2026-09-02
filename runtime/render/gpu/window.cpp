#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"

namespace PWEngine::Render
{
    Window* Instance::CreateWindow(WindowInfo info, Device* device)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,
                       info.is_resizable ? GLFW_TRUE : GLFW_FALSE);
        auto window = glfwCreateWindow(
            info.size.x, info.size.y, info.title.c_str(), nullptr, nullptr);
        
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(this->instance, window, nullptr, &surface) !=
            VK_SUCCESS)
        {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create window surface!");
        }
        Window* self = new Window();
        self->ptr = window;
        self->surface = surface;
        device->windows.push_back(self);
        self->p_device = device;
        return self;
    }

    bool Window::isClosed()
    {
        return glfwWindowShouldClose(this->ptr);
    }
} // namespace PWEngine::Render