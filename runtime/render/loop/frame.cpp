#include "render.hpp"
#include "./render/_render.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    /*
        Get semaphores to wait the GPU work and submit command buffer.
    */
    void RenderContext::frameSubmit(size_t swapchain_loop_frame_index, uint32_t* swapchain_image_index)
    {
        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = {this->self->image_available_semaphores[swapchain_loop_frame_index]};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &this->self->command_buffers[swapchain_loop_frame_index];

        VkSemaphore signal_semaphores[] = {this->self->render_finished_semaphores[swapchain_loop_frame_index]};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        if (vkQueueSubmit(this->m_device->graphics_queue, 1, &submit_info, this->self->in_flight_fences[swapchain_loop_frame_index]) != VK_SUCCESS) {
            Stream::log(this->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to submit draw command buffer!");
        }

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = {this->m_swapchain->swapchain};
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = swapchain_image_index;

        vkQueuePresentKHR(this->m_device->graphics_queue, &present_info);
    }
}