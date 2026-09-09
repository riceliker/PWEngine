#include "render.hpp"
#include "impl.hpp"
#include "../gpu/impl.hpp"
#include "../command/impl.hpp"
#include "buffer.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
namespace PWEngine::Render
{
    
    Mesh2D::Mesh2D(): self(std::make_unique<Impl>())
    {

    }

    void Mesh2D::setVertices()
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

        copyBufferCommand(this->p_context, staging_buffer, vertex_buffer, size);

        vkDestroyBuffer(this->p_context->self->device, staging_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, staging_buffer_memory, nullptr);

        this->self->vertex_buffer = vertex_buffer;
        this->self->vertex_buffer_memory = vertex_buffer_memory;
    }
    void Mesh2D::setIndices()
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

        copyBufferCommand(this->p_context, staging_buffer, index_buffer, size);

        vkDestroyBuffer(this->p_context->self->device, staging_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, staging_buffer_memory, nullptr);
        
        this->self->indices_buffer = index_buffer;
        this->self->indices_buffer_memory = index_buffer_memory;
    }

    std::unique_ptr<Mesh2D> RenderContext::createMesh2D(std::vector<Utils::Vertex2D> vertices, std::vector<uint32_t> indices)
    {
        std::unique_ptr<Mesh2D> obj = std::make_unique<Mesh2D>();
        obj->p_context = this;
        obj->vertices = std::move(vertices);
        obj->indices = std::move(indices);
        obj->setVertices();
        obj->setIndices();
        return obj;
    }

    Mesh2D::~Mesh2D()
    {
        vkDestroyBuffer(this->p_context->self->device, this->self->vertex_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, this->self->vertex_buffer_memory, nullptr);

        vkDestroyBuffer(this->p_context->self->device, this->self->indices_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, this->self->indices_buffer_memory, nullptr);
    }

    
}