#include "gpu.hpp"
#include "_memory.hpp"
#include "utils.hpp"
#include <cstddef>
#include <memory>
namespace PWEngine::GPU 
{  
    GPUBindingBuffer::GPUBindingBuffer(GPUInstance* instance, BufferUsageMask usage, size_t size, void* data) : IGPUBuffer()
    {
        this->p_instance = instance;
        VkBuffer buffer;
        VkDeviceMemory memory;

        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = getBufferUsage(usage);
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(p_instance->self->device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        
        VkMemoryAllocateInfo alloc_info = getBufferAllocatorInfo(p_instance, buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(p_instance->self->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        vkBindBufferMemory(p_instance->self->device, buffer, memory, 0);

        vkMapMemory(p_instance->self->device, memory, 0, size, 0, &this->map_pointer);

        this->self->buffer = buffer;
        this->self->memory = memory;
    }

    GPUBindingBuffer::~GPUBindingBuffer()
    {
        vkDestroyBuffer(this->p_instance->self->device, this->self->buffer, nullptr);
        vkFreeMemory(this->p_instance->self->device, this->self->memory, nullptr);
    }
}