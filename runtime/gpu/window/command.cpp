#include "gpu.hpp"
#include "_window.hpp"
#include "gpu/instance/_instance.hpp"
#include <cstdint>
#include <memory>

namespace PWEngine::GPU
{
    void WindowTask::createCommandPool()
    {
        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;
        command_buffers.resize(flight_fences_index_counts);

        QueueFamilyIndices queue_family_indices = findQueueFamilies(this->p_instance->self->adapter);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

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

    void WindowTask::beginCommandInProcess()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(this->self->command_buffers[this->flight_fences_index], &beginInfo) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to begin recording command buffer!");
    }
    
    void WindowTask::endCommandInProcess()
    {
        if (vkEndCommandBuffer(this->self->command_buffers[this->flight_fences_index]) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to record command buffer!");
    }
}