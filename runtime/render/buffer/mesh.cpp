#include "render.hpp"
#include "impl.hpp"
#include "../gpu/impl.hpp"
#include "buffer.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
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
    Mesh::Mesh(): self(std::make_unique<Impl>())
    {

    }

    void Mesh::setVertices()
    {
        size_t size = sizeof(this->vertices[0]) * this->vertices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createStagingBuffer(this->p_context, size, staging_buffer, staging_buffer_memory);

        /* data */
        void* data;
        vkMapMemory(this->p_context->self->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, this->vertices.data(), (size_t) size);
        vkUnmapMemory(this->p_context->self->device, staging_buffer_memory);

        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        createRealBuffer(this->p_context, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer, vertex_buffer_memory);

        copyBuffer(this->p_context, staging_buffer, vertex_buffer, size);

        vkDestroyBuffer(this->p_context->self->device, staging_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, staging_buffer_memory, nullptr);

        this->self->vertex_buffer = vertex_buffer;
        this->self->vertex_buffer_memory = vertex_buffer_memory;
    }
    void Mesh::setIndices()
    {
        size_t size = sizeof(this->indices[0]) * this->indices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createStagingBuffer(this->p_context, size, staging_buffer, staging_buffer_memory);

        /* data */
        void* data;
        vkMapMemory(this->p_context->self->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, this->indices.data(), (size_t) size);
        vkUnmapMemory(this->p_context->self->device, staging_buffer_memory);

        VkBuffer index_buffer;
        VkDeviceMemory index_buffer_memory;
        createRealBuffer(this->p_context, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer, index_buffer_memory);

        copyBuffer(this->p_context, staging_buffer, index_buffer, size);

        vkDestroyBuffer(this->p_context->self->device, staging_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, staging_buffer_memory, nullptr);
        
        this->self->indices_buffer = index_buffer;
        this->self->indices_buffer_memory = index_buffer_memory;
    }

    std::unique_ptr<Mesh> RenderContext::createMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    {
        std::unique_ptr<Mesh> obj = std::make_unique<Mesh>();
        obj->p_context = this;
        obj->vertices = std::move(vertices);
        obj->indices = std::move(indices);
        obj->setVertices();
        obj->setIndices();
        return obj;
    }

    Mesh::~Mesh()
    {
        vkDestroyBuffer(this->p_context->self->device, this->self->vertex_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, this->self->vertex_buffer_memory, nullptr);

        vkDestroyBuffer(this->p_context->self->device, this->self->indices_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, this->self->indices_buffer_memory, nullptr);
    }

    
}