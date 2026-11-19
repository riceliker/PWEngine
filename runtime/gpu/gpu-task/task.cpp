#include "gpu.hpp"
#include "gpu/instance/_instance.hpp"
#include "_task.hpp"
#include <memory>

namespace PWEngine::GPU 
{
    GPUTask::GPUTask(GPUInstance* instance) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;

        this->createSync();
        this->createCommandPool();
    }

    GPUTask::~GPUTask()
    {
        vkDestroyCommandPool(this->p_instance->self->device, this->self->command_pool, nullptr);
        vkDestroySemaphore(this->p_instance->self->device, this->self->compute_finished_semaphore, nullptr);
        vkDestroyFence(this->p_instance->self->device, this->self->in_flight_fence, nullptr);
    }

    void GPUTask::createSync()
    {
        VkSemaphore compute_finished_semaphore;
        VkFence in_flight_fence;

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(this->p_instance->self->device, &semaphore_info, nullptr, &this->self->compute_finished_semaphore) != VK_SUCCESS ||
            vkCreateFence(this->p_instance->self->device, &fence_info, nullptr, &this->self->in_flight_fence) != VK_SUCCESS) 
                sendLog(this->p_instance->log, LogType::Error, LogFrom::Vulkan, "failed to create synchronization objects for a frame!");
    }

    void GPUTask::run()
    {
        /* reset */
        vkResetFences(this->p_instance->self->device, 1, &this->self->in_flight_fence);
        vkResetCommandBuffer(this->self->command_buffer, /*VkCommandBufferResetFlagBits*/ 0);

        /* run */
        GPURecordCommand command(this);
        this->beginCommandInProcess();
        this->process(&command);
        this->endCommandInProcess();

        /* sumbit */
        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore signal_semaphores[] = {this->self->compute_finished_semaphore};
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        /* notice: the graphic queue is not thread safety. */
        this->p_instance->mutexVisit([this, submit_info, present_info](){
            if (vkQueueSubmit(this->p_instance->self->graphics_queue, 1, &submit_info, this->self->in_flight_fence) != VK_SUCCESS)
                sendLog(this->p_instance->log, LogType::Error, LogFrom::Vulkan, "failed to submit draw command buffer!");

        }); 

        /* wait GPU and reset */
        vkWaitForFences(this->p_instance->self->device, 1, &this->self->in_flight_fence, VK_TRUE, UINT64_MAX);
    } 
}