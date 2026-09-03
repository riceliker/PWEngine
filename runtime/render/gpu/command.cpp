#include "render.hpp"
#include "checker.hpp"

namespace PWEngine::Render 
{
    CommandPool* Device::createCommandPool()
    {
        VkCommandPool commandPool;
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(this->adapter);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(this->ptr, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            Stream::log(this->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create command pool!");
        }

        CommandPool* self = new CommandPool();
        self->command_pool = commandPool;
        self->p_device = this;
        this->command_pools.push_back(self);
        return self;
    }

    CommandPool::~CommandPool()
    {
        vkDestroyCommandPool(this->p_device->ptr, this->command_pool, nullptr);
    }

    CommandBuffer* CommandPool::createBuffer()
    {
        VkCommandBuffer commandBuffer;
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = this->command_pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(this->p_device->ptr, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            Stream::log(this->p_device->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to allocate command buffers!");
        }

        CommandBuffer* self = new CommandBuffer();
        self->commandBuffer = commandBuffer;
        self->p_pool = this;
        this->command_buffers.push_back(self);
        return self;
    }
}