#include "render.hpp"
#include "../gpu/impl.hpp"

namespace PWEngine::Render 
{
    inline uint32_t findMemoryType(RenderContext* context, uint32_t type_filter, VkMemoryPropertyFlags properties) 
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(context->self->adapter, &mem_properties);
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
    
    inline void allocatorGPUMemory(RenderContext* context, VkBuffer& buffer, VkDeviceMemory& memory)
    {
        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(context->self->device, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(context->self->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) 
        {
            Stream::log(context->log, Stream::LogType::Warn, Stream::LogFrom::VulkanRender, "failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(context->self->device, buffer, memory, 0);
    }

    inline void allocatorGPUMemory(RenderContext* context, VkImage& image, VkDeviceMemory& memory)
    {
        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(context->self->device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(context->self->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(context->self->device, image, memory, 0);
    }
}