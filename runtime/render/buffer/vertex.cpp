#include "render.hpp"
#include "impl.hpp"
#include "../gpu/impl.hpp"
#include "buffer.hpp"
#include <algorithm>
#include <memory>
#include <vector>
namespace PWEngine::Render
{
    VertexBuffer::VertexBuffer(): self(std::make_unique<Impl>())
    {

    }

    std::unique_ptr<VertexBuffer> RenderContext::createVertexBuffer(std::vector<Vertex> vertices)
    {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        /* vertex buffer */
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(vertices[0]) * vertices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(this->self->device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }
        /* vertex buffer memory */
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(this->self->device, vertexBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(this, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(this->self->device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(this->self->device, vertexBuffer, vertexBufferMemory, 0);

        /* data */
        void* data;
        vkMapMemory(this->self->device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferInfo.size);
        vkUnmapMemory(this->self->device, vertexBufferMemory);

        std::unique_ptr<VertexBuffer> obj = std::make_unique<VertexBuffer>();
        obj->self->vertexBuffer = vertexBuffer;
        obj->self->vertexBufferMemory = vertexBufferMemory;
        obj->vertices = std::move(vertices);
        obj->p_context = this;
        return obj;
    }

    VertexBuffer::~VertexBuffer()
    {
        vkDestroyBuffer(this->p_context->self->device, this->self->vertexBuffer, nullptr);
        vkFreeMemory(this->p_context->self->device, this->self->vertexBufferMemory, nullptr);
    }

    
}