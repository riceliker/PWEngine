#include "render.hpp"
#include "impl.hpp"

namespace PWEngine::Render
{
    void RenderContext::waitFence()
    {
        vkWaitForFences(this->m_device->device, 1, &this->self->in_flight_fences[this->current_frame], VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(this->m_device->device, this->m_swapchain->swapchain, UINT64_MAX, this->self->image_available_semaphores[current_frame], VK_NULL_HANDLE, &this->image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            this->recreateSwapchain();
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(this->m_device->device, 1, &this->self->in_flight_fences[this->current_frame]);
        vkResetCommandBuffer(this->self->command_buffers[this->current_frame], /*VkCommandBufferResetFlagBits*/ 0);

    }

    bool RenderContext::getIsWindowClosed()
    {
        return glfwWindowShouldClose(this->m_window->window);
    }

    void RenderContext::waitIdle()
    {
        vkDeviceWaitIdle(this->m_device->device);
    }

    void RenderContext::pollEvents()
    {
        glfwPollEvents();
    }
}