#include "_allocator.hpp"
namespace PWEngine::GPU 
{
    void GPUBuffer::createGPUBuffer(size_t element_total_size, void* element)
    {
        size_t size = element_total_size;
        CreateBufferInfo staging_info = {
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            size
        };
        auto [staging_buffer, staging_buffer_memory] = createBuffer(p_instance, staging_info).value();

        /* data */
        void* data;
        vkMapMemory(p_instance->self->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, element, size);
        vkUnmapMemory(p_instance->self->device, staging_buffer_memory);

        CreateBufferInfo vertex_info = {
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            size
        };
        auto [vertex_buffer, vertex_buffer_memory] = createBuffer(p_instance, vertex_info).value();

        copyBufferCommand(p_instance, staging_buffer, vertex_buffer, size);

        vkDestroyBuffer(p_instance->self->device, staging_buffer, nullptr);
        vkFreeMemory(p_instance->self->device, staging_buffer_memory, nullptr);

        this->self->buffer = vertex_buffer;
        this->self->memory = vertex_buffer_memory;
    }    
}