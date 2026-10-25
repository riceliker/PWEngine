#include "gpu.hpp"
#include "_task.hpp"
#include "gpu/instance/_instance.hpp"
#include <memory>

namespace PWEngine::GPU 
{
    IGPUTask::IGPUTask(GPUInstance* instance) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;
        this->createSync();
        this->createCommandPool();
    }

    IGPUTask::~IGPUTask()
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

    void IGPUTask::beginCommandInProcess()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(this->self->command_buffers[this->flight_fences_index], &beginInfo) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to begin recording command buffer!");
    }
    
    void IGPUTask::endCommandInProcess()
    {
        if (vkEndCommandBuffer(this->self->command_buffers[this->flight_fences_index]) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to record command buffer!");
    }

    void IGPUTask::createSync()
    {
        std::vector<VkSemaphore> image_available_semaphores;
        std::vector<VkSemaphore> render_finished_semaphores;
        std::vector<VkFence> in_flight_fences;

        image_available_semaphores.resize(this->flight_fences_index_counts);
        render_finished_semaphores.resize(this->flight_fences_index_counts);
        in_flight_fences.resize(this->flight_fences_index_counts);

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < this->flight_fences_index_counts; i++) {
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

    void IGPUTask::createCommandPool()
    {
        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;
        command_buffers.resize(this->flight_fences_index_counts);

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(this->p_instance->self->adapter);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(this->p_instance->self->device, &pool_info, nullptr, &command_pool) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create command pool!");

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = (uint32_t) command_buffers.size();

        if (vkAllocateCommandBuffers(this->p_instance->self->device, &alloc_info, command_buffers.data()) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate command buffers!");

        this->self->command_pool = command_pool;
        this->self->command_buffers = std::move(command_buffers);
    }
}