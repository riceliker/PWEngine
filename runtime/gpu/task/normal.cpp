#include "gpu.hpp"
#include "_task.hpp"
#include "gpu/instance/_instance.hpp"

namespace PWEngine::GPU 
{
    GPUTask::GPUTask(GPUInstance* instance): IGPUTask(instance)
    {

    }

    GPUTask::~GPUTask()
    {
        
    }

    void GPUTask::waitFenceInProcess()
    {
        vkWaitForFences(this->p_instance->self->device, 1, &this->self->in_flight_fences[this->flight_fences_index], VK_TRUE, UINT64_MAX);
        vkResetFences(this->p_instance->self->device, 1, &this->self->in_flight_fences[this->flight_fences_index]);
        vkResetCommandBuffer(this->self->command_buffers[this->flight_fences_index], /*VkCommandBufferResetFlagBits*/ 0);
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to acquire swap chain image!");
    }

    bool GPUTask::sub_thread_loop(double delta)
    {
        this->waitFenceInProcess();
        this->beginCommandInProcess();
        bool result = this->loop(delta);
        this->endCommandInProcess();
        this->submitCommandInProcess();
        return result;
    }    

    void GPUTask::submitCommandInProcess()
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

        this->p_instance->mutexVisit([this, submit_info](){
            if (vkQueueSubmit(this->p_instance->self->graphics_queue, 1, &submit_info, this->self->in_flight_fences[this->flight_fences_index]) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to submit draw command buffer!");
        }); 
    }


}