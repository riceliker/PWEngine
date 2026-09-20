#pragma once
#include "render.hpp"
#include "../context/impl.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>

namespace PWEngine::Render 
{
    inline uint32_t findMemoryType(RenderContext* context, uint32_t type_filter, VkMemoryPropertyFlags properties) 
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(context->m_device->adapter, &mem_properties);
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    inline void createStagingBuffer(RenderContext* context, size_t size, VkBuffer& buffer, VkDeviceMemory& memory)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(context->m_device->device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(context->m_device->device, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(context->m_device->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(context->m_device->device, buffer, memory, 0);
    }

    inline void createRealBuffer(RenderContext* context, size_t size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(context->m_device->device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(context->m_device->device, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(context->m_device->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(context->m_device->device, buffer, memory, 0);
    }

    inline void createDirectBuffer(RenderContext* context, size_t size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(context->m_device->device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(context->m_device->device, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(context->m_device->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(context->m_device->device, buffer, memory, 0);
    }

    inline void createImageBuffer(RenderContext* context, Utils::Vec2<uint32_t> size, uint32_t mip_level, VkImage& image, VkDeviceMemory& memory)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = static_cast<uint32_t>(size.x);
        image_info.extent.height = static_cast<uint32_t>(size.y);
        image_info.extent.depth = 1;
        image_info.mipLevels = mip_level;
        image_info.arrayLayers = 1;
        image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional

        if (vkCreateImage(context->m_device->device, &image_info, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(context->m_device->device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(context->m_device->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindImageMemory(context->m_device->device, image, memory, 0);
    }

    inline void createDepthBuffer(RenderContext* context, VkExtent2D swapchain_extent, VkImage& image, VkDeviceMemory& memory)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = swapchain_extent.width;
        image_info.extent.height = swapchain_extent.height;
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL; //1
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; //2
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional

        if (vkCreateImage(context->m_device->device, &image_info, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(context->m_device->device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(context, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); //3

        if (vkAllocateMemory(context->m_device->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindImageMemory(context->m_device->device, image, memory, 0);
    }
    
    inline std::vector<VkDescriptorSet> createDescriptorSet(Pipeline3D* pipeline, VkDescriptorSetLayout layout)
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = pipeline->p_context->self->descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        alloc_info.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptor_set;

        descriptor_set.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(pipeline->p_context->m_device->device, &alloc_info, descriptor_set.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        return descriptor_set;
    }


}