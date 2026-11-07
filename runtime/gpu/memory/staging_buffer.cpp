#include "gpu.hpp"
#include "_memory.hpp"
#include "utils.hpp"
#include <cstddef>
#include <memory>
namespace PWEngine::GPU 
{
    GPUStagingBuffer::GPUStagingBuffer(GPUInstance* instance, BufferUsageMask usage, size_t size, void* data) : IGPUBuffer()
    {
        this->p_instance = instance;

        /* create staging buffer */
        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
 
        VkBufferCreateInfo staging_buffer_info{};
        staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        staging_buffer_info.size = size;
        staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(p_instance->self->device, &staging_buffer_info, nullptr, &staging_buffer) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        
        VkMemoryAllocateInfo staging_alloc_info = getBufferAllocatorInfo(p_instance, staging_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(p_instance->self->device, &staging_alloc_info, nullptr, &staging_memory) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        vkBindBufferMemory(p_instance->self->device, staging_buffer, staging_memory, 0);

        /* copy data */
        void* temp;
        vkMapMemory(p_instance->self->device, staging_memory, 0, size, 0, &temp);
        memcpy(temp, data, size);
        vkUnmapMemory(p_instance->self->device, staging_memory);

        /* create real buffer */
        VkBuffer buffer;
        VkDeviceMemory memory;

        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = getBufferUsage(usage);
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(p_instance->self->device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        
        VkMemoryAllocateInfo alloc_info = getBufferAllocatorInfo(p_instance, buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        if (vkAllocateMemory(p_instance->self->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        vkBindBufferMemory(p_instance->self->device, buffer, memory, 0);

        copyBufferCommand(p_instance, staging_buffer, buffer, {0, 0, size});
        vkMapMemory(p_instance->self->device, memory, 0, size, 0, &this->map_ptr);

        vkDestroyBuffer(p_instance->self->device, staging_buffer, nullptr);
        vkFreeMemory(p_instance->self->device, staging_memory, nullptr);

        this->self->buffer = buffer;
        this->self->memory = memory;
    } 

    void GPUStagingBuffer::readData(void* data, size_t offset, size_t size)
    {
        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
 
        VkBufferCreateInfo staging_buffer_info{};
        staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        staging_buffer_info.size = size;
        staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(p_instance->self->device, &staging_buffer_info, nullptr, &staging_buffer) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        
        VkMemoryAllocateInfo staging_alloc_info = getBufferAllocatorInfo(p_instance, staging_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(p_instance->self->device, &staging_alloc_info, nullptr, &staging_memory) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        vkBindBufferMemory(p_instance->self->device, staging_buffer, staging_memory, 0);

        /* copy data */
        void* temp;
        vkMapMemory(p_instance->self->device, staging_memory, 0, size, 0, &temp);
        memcpy(temp, data, size);
        vkUnmapMemory(p_instance->self->device, staging_memory);

        copyBufferCommand(p_instance, staging_buffer, this->self->buffer, {0, offset, size});
        vkDestroyBuffer(p_instance->self->device, staging_buffer, nullptr);
    }

    void GPUStagingBuffer::writeData(void* data, size_t offset, size_t size)
    {
        /* create staging buffer */
        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
 
        VkBufferCreateInfo staging_buffer_info{};
        staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        staging_buffer_info.size = size;
        staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(p_instance->self->device, &staging_buffer_info, nullptr, &staging_buffer) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        
        VkMemoryAllocateInfo staging_alloc_info = getBufferAllocatorInfo(p_instance, staging_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(p_instance->self->device, &staging_alloc_info, nullptr, &staging_memory) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        vkBindBufferMemory(p_instance->self->device, staging_buffer, staging_memory, 0);

        /* copy data */
        copyBufferCommand(p_instance, this->self->buffer, staging_buffer, {offset, 0, size});

        void* temp;
        vkMapMemory(p_instance->self->device, staging_memory, 0, size, 0, &temp);
        memcpy(data, temp, size);
        vkUnmapMemory(p_instance->self->device, staging_memory);

        vkDestroyBuffer(p_instance->self->device, staging_buffer, nullptr);
    }
    
    GPUStagingBuffer::~GPUStagingBuffer()
    {
        vkDestroyBuffer(this->p_instance->self->device, this->self->buffer, nullptr);
        vkFreeMemory(this->p_instance->self->device, this->self->memory, nullptr);
    }
}