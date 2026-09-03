#include "render.hpp"
#include "stream.hpp"
#include "utils.hpp"

namespace PWEngine::Render
{
    Window* Device::createWindow(WindowInfo info)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,
                       info.is_resizable ? GLFW_TRUE : GLFW_FALSE);
        auto window = glfwCreateWindow(
            info.size.x, info.size.y, info.title.c_str(), nullptr, nullptr);
        
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(this->p_instance->ptr, window, nullptr, &surface) !=
            VK_SUCCESS)
        {
            Stream::log(this->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create window surface!");
        }
        Window* self = new Window();
        self->ptr = window;
        self->surface = surface;
        self->p_device = this;
        this->windows.push_back(self);
        return self;
    }

    Window::~Window()
    {
        for (const auto& swapchain: swapchains)
        {
            delete swapchain;
        }

        vkDestroySurfaceKHR(this->p_device->p_instance->ptr, surface, nullptr);
        glfwDestroyWindow(this->ptr);
    }

    bool Window::isClosed()
    {
        return glfwWindowShouldClose(this->ptr);
    }
} // namespace PWEngine::Render