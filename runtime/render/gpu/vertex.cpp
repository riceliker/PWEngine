#include "render.hpp"
#include "impl.hpp"
#include "checker.hpp"
#include <vector>
namespace PWEngine::Render
{
    VkVertexInputBindingDescription Vertex::getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() 
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }

    VertexBuffer* Device::createVertexBuffer(std::vector<Vertex> vertices)
    {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        /* vertex buffer */
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(vertices[0]) * vertices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(this->self->ptr, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }
        /* vertex buffer memory */
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(this->self->ptr, vertexBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(this, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(this->self->ptr, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(this->self->ptr, vertexBuffer, vertexBufferMemory, 0);

        /* data */
        void* data;
        vkMapMemory(this->self->ptr, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferInfo.size);
        vkUnmapMemory(this->self->ptr, vertexBufferMemory);

        VertexBuffer* self = new VertexBuffer();
        self->vertexBuffer = vertexBuffer;
        self->vertexBufferMemory = vertexBufferMemory;
        self->vertices = std::move(vertices);
        self->p_device = this;
        this->self->vertex_buffers.push_back(self);
        return self;
    }

    VertexBuffer::~VertexBuffer()
    {
        vkDestroyBuffer(this->p_device->self->ptr, vertexBuffer, nullptr);
        vkFreeMemory(this->p_device->self->ptr, vertexBufferMemory, nullptr);
    }

    
}