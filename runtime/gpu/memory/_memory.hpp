// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#pragma once
/* include library */
#include <cstdint>
#include <string>
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
    struct CreateImageInfo
    {
        VkMemoryPropertyFlags properties;
        Vec2<uint32_t> size;
        uint32_t mip_level;
        ImageFormat format;
    };

    struct IGPUBuffer::Impl
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
    };

    struct GPUImage::Impl
    {
        uint32_t mip_level;
        VkImage image;
        VkDeviceMemory memory;
    };

    struct GPUImageView::Impl
    {
        VkImageView view;
        VkSampler sampler;
    };

    VkMemoryAllocateInfo getBufferAllocatorInfo(GPUInstance* instance, VkBuffer& buffer, VkMemoryPropertyFlags properties);
    VkMemoryAllocateInfo getImageAllocatorInfo(GPUInstance* instance, VkImage& image, VkMemoryPropertyFlags properties);
    VkBufferUsageFlags getBufferUsage(BufferUsageMask usage);

    void copyBufferCommand(GPUInstance* instance, VkBuffer staging, VkBuffer real, VkBufferCopy copy_region);
    void copyBufferToImageCommand(GPUInstance* instance, VkBuffer buffer, VkImage image, Vec2<uint32_t> size);
    void transitionImageLayoutCommand(GPUInstance* instance, uint32_t mip_level, VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
    void generateMipmapCommand(GPUInstance* instance, uint32_t mip_level, Vec2<uint32_t> size, VkImage image);

    inline std::optional<std::tuple<VkImage, VkDeviceMemory>> createDepthImage(GPUInstance* instance, CreateImageInfo info)
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
        case ImageFormat::RGBA8_SRGB:
            image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        break;
        case ImageFormat::RGB8_SRGB:
            image_info.format = VK_FORMAT_R8G8B8_SRGB;
        break;
        case ImageFormat::D_F32_S_U8:
            image_info.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
        }
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
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
        for (index = 0; index < mem_properties.memoryTypeCount; ++index) 
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
}