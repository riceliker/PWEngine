#include "gpu.hpp"
#include "_allocator.hpp"
#include <cstddef>
#include <memory>
namespace PWEngine::GPU 
{
    GPUBinding::GPUBinding(GPUInstance* instance, GPUBindingDataType type, size_t size) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;
        this->createGPUBinding(type, size);
        
    }

    GPUBinding::~GPUBinding()
    {
        vkDestroyBuffer(this->p_instance->self->device, this->self->buffer, nullptr);
        vkFreeMemory(this->p_instance->self->device, this->self->memory, nullptr);
    }

    void GPUBinding::createGPUBinding(GPUBindingDataType type, size_t size)
    {
        VkBufferUsageFlags usage;
        switch (type) 
        {
        case GPUBindingDataType::uniform:
            usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
        case GPUBindingDataType::storage:
            usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        break;
        }

        CreateBufferInfo staging_info{};
        staging_info.size = size;
        staging_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging_info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        auto [binding_buffer, binding_memory] = createDirectBuffer(this->p_instance, size, usage, staging_info).value();
        vkMapMemory(this->p_instance->self->device, binding_memory, 0, size, 0, &this->map_pointer);

        this->self->buffer = binding_buffer;
        this->self->memory = binding_memory;
    }

    void GPUBinding::writeData(void* data, size_t size)
    {
        memcpy(this->map_pointer, data, size);
    }

    void GPUBinding::getData(void* data, size_t size)
    {
        memcpy(data, this->map_pointer, size);
    }
}