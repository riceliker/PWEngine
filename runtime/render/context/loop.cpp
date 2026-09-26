#include "render.hpp"
#include "impl.hpp"
#include <cstddef>
#include <cstdint>

namespace PWEngine::Render
{
    void RenderContext::waitFence(size_t swapchain_loop_frame_index, uint32_t* swapchain_image_index)
    {
        vkWaitForFences(this->m_device->device, 1, &this->self->in_flight_fences[swapchain_loop_frame_index], VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(this->m_device->device, this->m_swapchain->swapchain, UINT64_MAX, this->self->image_available_semaphores[swapchain_loop_frame_index], VK_NULL_HANDLE, swapchain_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            this->recreateSwapchain();
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(this->m_device->device, 1, &this->self->in_flight_fences[swapchain_loop_frame_index]);
        vkResetCommandBuffer(this->self->command_buffers[swapchain_loop_frame_index], /*VkCommandBufferResetFlagBits*/ 0);

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