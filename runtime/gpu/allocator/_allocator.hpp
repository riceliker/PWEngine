// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#pragma once
/* include library */
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
/* header */
#include "gpu.hpp"
#include "gpu/instance/_instance.hpp"
/* std library */
#include <cstddef>
#include <optional>
#include <tuple>

namespace PWEngine::GPU
{
    struct GPUBuffer::Impl
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
    };

    struct GPUImage::Impl
    {
        uint32_t mip_level;
        VkImage texture_image;
        VkDeviceMemory texture_image_memory;
    };

    inline uint32_t findMemoryType(GPUInstance* instance, uint32_t type_filter, VkMemoryPropertyFlags properties) 
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(instance->self->adapter, &mem_properties);
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) 
        {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
            
        }
        sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to find suitable memory type!");
        return 0;
    }

    struct CreateBufferInfo
    {
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;
        size_t size;
    };

    struct CreateImageInfo
    {
        VkMemoryPropertyFlags properties;
        Vec2<uint32_t> size;
        uint32_t mip_level;
        ImageFormat format;
    };

    inline std::optional<std::tuple<VkBuffer, VkDeviceMemory>> createBuffer(GPUInstance* instance, CreateBufferInfo info)
    {
        VkBuffer buffer;
        VkDeviceMemory memory;

        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = buffer_info.size;
        buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(instance->self->device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) 
            return std::nullopt;

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(instance->self->device, buffer, &mem_requirements);

        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(instance->self->adapter, &mem_properties);
        uint32_t index;
        for (uint32_t index = 0; index < mem_properties.memoryTypeCount; ++index) 
        {
            if ((mem_requirements.memoryTypeBits & (1 << index)) && (mem_properties.memoryTypes[index].propertyFlags & info.properties) == info.properties)
                goto get_memory_type_index_success;
        }
        sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to find suitable memory type!");
        get_memory_type_index_success:
        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = index;
        
        if (vkAllocateMemory(instance->self->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) 
            sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");

        vkBindBufferMemory(instance->self->device, buffer, memory, 0);

        return std::make_tuple(buffer, memory);
    }

    inline std::optional<std::tuple<VkImage, VkDeviceMemory>> createImage(GPUInstance* instance, CreateImageInfo info)
    {
        VkImage image;
        VkDeviceMemory memory;

        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = static_cast<uint32_t>(info.size.x);
        image_info.extent.height = static_cast<uint32_t>(info.size.y);
        image_info.extent.depth = 1;
        image_info.mipLevels = info.mip_level;
        image_info.arrayLayers = 1;
        switch (info.format) 
        {
        case ImageFormat::RGBA8:
            image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        break;
        case ImageFormat::RGB8:
            image_info.format = VK_FORMAT_R8G8B8_SRGB;
        break;
        case ImageFormat::D_F32_S_U8:
            image_info.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
        }
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional

        if (vkCreateImage(instance->self->device, &image_info, nullptr, &image) != VK_SUCCESS) 
            sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create image!");
        
        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(instance->self->device, image, &mem_requirements);

        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(instance->self->adapter, &mem_properties);
        uint32_t index;
        for (uint32_t index = 0; index < mem_properties.memoryTypeCount; ++index) 
        {
            if ((mem_requirements.memoryTypeBits & (1 << index)) && (mem_properties.memoryTypes[index].propertyFlags & info.properties) == info.properties)
                goto get_memory_type_index_success;
        }
        sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to find suitable memory type!");
        return std::nullopt;
        get_memory_type_index_success:
        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = index;
        
        if (vkAllocateMemory(instance->self->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) 
            sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");

        vkBindImageMemory(instance->self->device, image, memory, 0);
        return std::make_tuple(image, memory);
    }

    template<std::invocable<VkCommandBuffer&> F> inline void instanceCommand(GPUInstance* instance, F&& func)
    {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = instance->self->command_pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(instance->self->device, &alloc_info, &command_buffer);
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
        func(command_buffer);
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(instance->self->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(instance->self->graphics_queue);

        vkFreeCommandBuffers(instance->self->device, instance->self->command_pool, 1, &command_buffer);
    
    }

    inline void copyBufferCommand(GPUInstance* instance, VkBuffer staging, VkBuffer real, size_t size)
    {
        instanceCommand(instance, [&](VkCommandBuffer& command_buffer){
            VkBufferCopy copy_region{};
            copy_region.srcOffset = 0; // Optional
            copy_region.dstOffset = 0; // Optional
            copy_region.size = size;
            vkCmdCopyBuffer(command_buffer, staging, real, 1, &copy_region);
        });
    }

    

    // inline void createDirectBuffer(GPUInstance* instance, size_t size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory)
    // {
    //     VkBufferCreateInfo buffer_info{};
    //     buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //     buffer_info.size = size;
    //     buffer_info.usage = usage;
    //     buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    //     if (vkCreateBuffer(context->m_device->device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) 
    //         sendLog(context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create buffer!");
        

    //     VkMemoryRequirements mem_requirements;
    //     vkGetBufferMemoryRequirements(context->m_device->device, buffer, &mem_requirements);

    //     VkMemoryAllocateInfo alloc_info{};
    //     alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     alloc_info.allocationSize = mem_requirements.size;
    //     alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    //     if (vkAllocateMemory(context->m_device->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) 
    //         sendLog(context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        
    //     vkBindBufferMemory(context->m_device->device, buffer, memory, 0);
    // }

    

    
    
    
}