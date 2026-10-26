#include "_allocator.hpp"
#include "gpu.hpp"
#include <memory>
namespace PWEngine::GPU 
{
    GPUBuffer::GPUBuffer(GPUInstance* p_instance, GPUBufferType type, size_t byte_size, void* data) : self(std::make_unique<Impl>())
    {
        this->p_instance = p_instance;
        createGPUBuffer(byte_size, data, type);
    }

    void GPUBuffer::createGPUBuffer(size_t size, void* element, GPUBufferType type)
    {
        CreateBufferInfo staging_info{};
        staging_info.size = size;
        staging_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging_info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
 
        auto [staging_buffer, staging_buffer_memory] = createBuffer(p_instance, staging_info).value();

        /* data */
        void* data;
        vkMapMemory(p_instance->self->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, element, size);
        vkUnmapMemory(p_instance->self->device, staging_buffer_memory);

        CreateBufferInfo vertex_info{};
        vertex_info.size = size;
        switch (type) 
        {
        case GPUBufferType::vertex:
            vertex_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
        case GPUBufferType::index:
            vertex_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
        }
        vertex_info.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        
        auto [vertex_buffer, vertex_buffer_memory] = createBuffer(p_instance, vertex_info).value();

        copyBufferCommand(p_instance, staging_buffer, vertex_buffer, size);

        vkDestroyBuffer(p_instance->self->device, staging_buffer, nullptr);
        vkFreeMemory(p_instance->self->device, staging_buffer_memory, nullptr);

        this->self->buffer = vertex_buffer;
        this->self->memory = vertex_buffer_memory;
    } 
    
    GPUBuffer::~GPUBuffer()
    {
        vkDestroyBuffer(this->p_instance->self->device, this->self->buffer, nullptr);
        vkFreeMemory(this->p_instance->self->device, this->self->memory, nullptr);
    }
}