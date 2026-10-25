#include "gpu.hpp"
#include "gpu/window/_window.hpp"
#include "gpu/instance/_instance.hpp"
#include "gpu/_format.hpp"
#include <cstdint>
#include <memory>

namespace PWEngine::GPU
{
    Window::Window(GPUInstance* instance, WindowInfo info) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;

        this->createWindow(info);
        Vec2<uint32_t> extent;
        this->createSwapchain(extent);
    }

    Window::~Window()
    {
        for (auto image_view : this->self->swapchain_image_views) {
            vkDestroyImageView(this->p_instance->self->device, image_view, nullptr);
        }
        vkDestroySwapchainKHR(this->p_instance->self->device, this->self->swapchain, nullptr);
        vkDestroySurfaceKHR(this->p_instance->self->instance, this->self->surface, nullptr);
        glfwDestroyWindow(this->self->window);
    }

    Format Window::getFormat()
    {
        return {this->self->swapchain_image_format};
    }

    static void focus_callback(GLFWwindow* window, int focused)
    {
        Window* win_ptr = (Window*)glfwGetWindowUserPointer(window);
        if (focused)
        {
            win_ptr->p_instance->focused_window = win_ptr;
            win_ptr->input = &win_ptr->p_instance->input;
        }
        else
        {
            if (win_ptr->p_instance->focused_window == win_ptr)
            {
                win_ptr->p_instance->focused_window = nullptr;
                win_ptr->input = nullptr;
            }   
        }
    }

    void Window::createWindow(WindowInfo info)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, info.is_window_resizable ? GLFW_TRUE : GLFW_FALSE);
        auto window = glfwCreateWindow(info.window_default_resolution.x, info.window_default_resolution.y, info.window_title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);

        glfwSetWindowFocusCallback(window, focus_callback);

        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(this->p_instance->self->instance, window, nullptr, &surface) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create window surface!");

        this->self->window = window;
        this->self->surface = surface;
    }
}