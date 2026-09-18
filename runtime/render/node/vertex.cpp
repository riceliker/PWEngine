#include "render.hpp"
#include "impl.hpp"
#include "../context/impl.hpp"
#include "../command/impl.hpp"
#include "buffer.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
namespace PWEngine::Render
{
    
    void Mesh3D::setVertices(RenderContext* super, std::vector<Utils::Vertex3D>& vertices)
    {
        size_t size = sizeof(vertices[0]) * vertices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createStagingBuffer(super, size, staging_buffer, staging_buffer_memory);

        /* data */
        void* data;
        vkMapMemory(super->m_device->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, vertices.data(), (size_t) size);
        vkUnmapMemory(super->m_device->device, staging_buffer_memory);

        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        createRealBuffer(super, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer, vertex_buffer_memory);

        copyBufferCommand(super, staging_buffer, vertex_buffer, size);

        vkDestroyBuffer(super->m_device->device, staging_buffer, nullptr);
        vkFreeMemory(super->m_device->device, staging_buffer_memory, nullptr);

        this->m_vertex->vertex_buffer = vertex_buffer;
        this->m_vertex->vertex_buffer_memory = vertex_buffer_memory;
    }

    void Mesh3D::setIndices(RenderContext* super, std::vector<uint32_t>& indices)
    {
        size_t size = sizeof(indices[0]) * indices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createStagingBuffer(super, size, staging_buffer, staging_buffer_memory);

        /* data */
        void* data;
        vkMapMemory(super->m_device->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, indices.data(), (size_t) size);
        vkUnmapMemory(super->m_device->device, staging_buffer_memory);

        VkBuffer index_buffer;
        VkDeviceMemory index_buffer_memory;
        createRealBuffer(super, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer, index_buffer_memory);

        copyBufferCommand(super, staging_buffer, index_buffer, size);

        vkDestroyBuffer(super->m_device->device, staging_buffer, nullptr);
        vkFreeMemory(super->m_device->device, staging_buffer_memory, nullptr);
        
        this->m_vertex->indices_buffer = index_buffer;
        this->m_vertex->indices_buffer_memory = index_buffer_memory;
    }
}