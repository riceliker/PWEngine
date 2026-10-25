#include "_instance.hpp"
namespace PWEngine::GPU 
{
    void GPUInstance::createCommandPool()
    {
        VkCommandPool command_pool;

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(this->self->adapter);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(this->self->device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS)
            sendLog(this->log, LogType::Error, LogFrom::VulkanRender, "failed to create command pool!");

        this->self->command_pool = command_pool;
    }
}