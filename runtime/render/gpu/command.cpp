#include "render.hpp"
#include "checker.hpp"

namespace PWEngine::Render 
{
    CommandPool* Device::createCommandPool()
    {
        VkCommandPool command_pool;
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(this->self->p_adapter);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(this->self->ptr, &poolInfo, nullptr, &command_pool) != VK_SUCCESS) {
            Stream::log(this->self->p_instance->self->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create command pool!");
        }

        CommandPool* self = new CommandPool();
        self->ptr = command_pool;
        self->p_device = this;
        this->self->command_pools.push_back(self);
        return self;
    }

    CommandPool::~CommandPool()
    {
        vkDestroyCommandPool(this->p_device->self->ptr, this->ptr, nullptr);
    }

    CommandBuffer* CommandPool::createBuffer()
    {
        VkCommandBuffer command_buffer;
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = this->ptr;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(this->p_device->self->ptr, &allocInfo, &command_buffer) != VK_SUCCESS) {
            Stream::log(this->p_device->self->p_instance->self->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to allocate command buffers!");
        }

        CommandBuffer* self = new CommandBuffer();
        self->ptr = command_buffer;
        self->p_pool = this;
        this->command_buffers.push_back(self);
        return self;
    }

    void CommandBuffer::bindPipeline(Pipeline* pipeline)
    {
        vkCmdBindPipeline(this->ptr, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphics_pipeline);
    }

    void CommandBuffer::setViewPort(Swapchain* swapchain)
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapchain->swapchain_extent.width;
        viewport.height = (float) swapchain->swapchain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(this->ptr, 0, 1, &viewport);
    }

    void CommandBuffer::setScissor(Swapchain* swapchain)
    {
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain->swapchain_extent;
        vkCmdSetScissor(this->ptr, 0, 1, &scissor);            
    }

    void CommandBuffer::addVertexBuffer(VertexBuffer* vertex_buffer)
    {
        VkBuffer vertexBuffers[] = {vertex_buffer->vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(this->ptr, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(this->ptr, static_cast<uint32_t>(vertex_buffer->vertices.size()), 1, 0, 0);
    }
}