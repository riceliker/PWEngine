#include "gpu.hpp"
#include "_window.hpp"
#include "gpu/instance/_instance.hpp"
#include "gpu/window/_window.hpp"
#include <memory>

namespace PWEngine::GPU 
{
    WindowTask::WindowTask(GPUInstance* instance, WindowInfo info) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;

        this->createSync();
        this->createCommandPool();
        this->m_window = std::make_unique<Window>(instance, info);
    }

    WindowTask::~WindowTask()
    {
        vkDestroyCommandPool(this->p_instance->self->device, this->self->command_pool, nullptr);
        /* createSync */
        for (auto render_finished_semaphore: this->self->render_finished_semaphores)
        {
            vkDestroySemaphore(this->p_instance->self->device, render_finished_semaphore, nullptr);
        }
        for (auto image_available_semaphore: this->self->image_available_semaphores)
        {
            vkDestroySemaphore(this->p_instance->self->device, image_available_semaphore, nullptr);
        }
        for (auto in_flight_fence: this->self->in_flight_fences)
        {
            vkDestroyFence(this->p_instance->self->device, in_flight_fence, nullptr);
        }
    }

    void WindowTask::createSync()
    {
        std::vector<VkSemaphore> image_available_semaphores;
        std::vector<VkSemaphore> render_finished_semaphores;
        std::vector<VkFence> in_flight_fences;

        image_available_semaphores.resize(flight_fences_index_counts);
        render_finished_semaphores.resize(flight_fences_index_counts);
        in_flight_fences.resize(flight_fences_index_counts);

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < flight_fences_index_counts; i++) {
            if (vkCreateSemaphore(this->p_instance->self->device, &semaphore_info, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(this->p_instance->self->device, &semaphore_info, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(this->p_instance->self->device, &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) 
            {
                sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create synchronization objects for a frame!");
            }
        }

        this->self->image_available_semaphores = std::move(image_available_semaphores);
        this->self->render_finished_semaphores = std::move(render_finished_semaphores);
        this->self->in_flight_fences = std::move(in_flight_fences);
    }

    bool WindowTask::task_loop(double delta)
    {
        /* check the window should closed. */
        if (this->m_window->input != nullptr && this->m_window->input->window_should_close.load()) return true;
        /* wait GPU finish */
        vkWaitForFences(this->p_instance->self->device, 1, &this->self->in_flight_fences[this->flight_fences_index], VK_TRUE, UINT64_MAX);
        /* get next swapchain image index and ouput result */
        VkResult swapchain_result = vkAcquireNextImageKHR(this->p_instance->self->device, this->m_window->self->swapchain, UINT64_MAX, this->self->image_available_semaphores[this->flight_fences_index], VK_NULL_HANDLE, &this->swapchain_image_index);
        if (swapchain_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            this->m_window->recreateSwapchain();
            return true;
        }   
        else if (swapchain_result != VK_SUCCESS && swapchain_result != VK_SUBOPTIMAL_KHR) 
        {
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to acquire swap chain image!");
            return true;
        } 
        /* resset fence */
        vkResetFences(this->p_instance->self->device, 1, &this->self->in_flight_fences[this->flight_fences_index]);
        vkResetCommandBuffer(this->self->command_buffers[this->flight_fences_index], /*VkCommandBufferResetFlagBits*/ 0);
        /* callback */
        bool result = this->process(delta);
        /* sumbit */
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
        present_info.pImageIndices = &swapchain_image_index;
        /* notice: the graphic queue is not thread safety. */
        this->p_instance->mutexVisit([this, submit_info, present_info](){
            if (vkQueueSubmit(this->p_instance->self->graphics_queue, 1, &submit_info, this->self->in_flight_fences[this->flight_fences_index]) != VK_SUCCESS)
                sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to submit draw command buffer!");
            if (vkQueuePresentKHR(this->p_instance->self->graphics_queue, &present_info) != VK_SUCCESS)
                sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to Present swapchain image!");
        }); 

        this->flight_fences_index = (this->flight_fences_index + 1) % flight_fences_index_counts;
        return result;
    }    
}