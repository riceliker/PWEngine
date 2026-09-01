#include "render.hpp"
#include "checker.hpp"

namespace PWEngine::Render::GPU 
{
    CommandBuffer::CommandBuffer(Device device, Window window)
    {
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        if (window.surface.has_value())
        {

        }
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(device.adapter, window.surface.value());

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device.ptr, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device.ptr, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        this->commandPool = commandPool;
        this->commandBuffer = commandBuffer;
    }
}