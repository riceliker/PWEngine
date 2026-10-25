#include "gpu.hpp"
#include "_task.hpp"
#include "gpu/instance/_instance.hpp"
#include "gpu/window/_window.hpp"
#include <memory>

namespace PWEngine::GPU 
{
    GPUTaskWithWindow::GPUTaskWithWindow(GPUInstance* instance, WindowInfo info) : IGPUTask(instance)
    {
        this->m_window = std::make_unique<Window>(instance, info);
    }

    GPUTaskWithWindow::~GPUTaskWithWindow()
    {
        
    }

    void GPUTaskWithWindow::waitFenceInProcess(uint32_t* swapchain_image_index)
    {
        vkWaitForFences(this->p_instance->self->device, 1, &this->self->in_flight_fences[this->flight_fences_index], VK_TRUE, UINT64_MAX);
        VkResult result = vkAcquireNextImageKHR(this->p_instance->self->device, this->m_window->self->swapchain, UINT64_MAX, this->self->image_available_semaphores[this->flight_fences_index], VK_NULL_HANDLE, swapchain_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            this->m_window->recreateSwapchain();
            return;
        }    
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
        {
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to acquire swap chain image!");
            return;
        } 
        
        vkResetFences(this->p_instance->self->device, 1, &this->self->in_flight_fences[this->flight_fences_index]);
        vkResetCommandBuffer(this->self->command_buffers[this->flight_fences_index], /*VkCommandBufferResetFlagBits*/ 0);
    }

    bool GPUTaskWithWindow::sub_thread_loop(double delta)
    {
        if (this->m_window->input != nullptr && this->m_window->input->window_should_close.load()) return true;
        uint32_t swapchain_image_index = 0;
        this->waitFenceInProcess(&swapchain_image_index);
        bool result = this->loop(delta);
        this->submitCommandInProcess(&swapchain_image_index);
        this->flight_fences_index = (this->flight_fences_index + 1) % this->flight_fences_index_counts;
        
        return result;
    }    

    void GPUTaskWithWindow::submitCommandInProcess(uint32_t* swapchain_image_index)
    {
        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore wait_semaphores[] = {this->self->image_available_semaphores[this->flight_fences_index]};
        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &this->self->command_buffers[this->flight_fences_index];

        VkSemaphore signal_semaphores[] = {this->self->render_finished_semaphores[this->flight_fences_index]};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = {this->m_window->self->swapchain};
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = swapchain_image_index;

        this->p_instance->mutexVisit([this, submit_info, present_info](){
            if (vkQueueSubmit(this->p_instance->self->graphics_queue, 1, &submit_info, this->self->in_flight_fences[this->flight_fences_index]) != VK_SUCCESS)
                sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to submit draw command buffer!");
            if (vkQueuePresentKHR(this->p_instance->self->graphics_queue, &present_info) != VK_SUCCESS)
                sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to Present swapchain image!");
        }); 
    }
}