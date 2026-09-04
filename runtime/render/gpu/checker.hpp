#pragma once
#include <vulkan/vulkan.h>
#if (__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
#include "render.hpp"
#include "impl.hpp"

#include <optional>
#include <vector>

namespace PWEngine::Render 
{
    const std::vector<const char*> device_extensions = {
    #if (__APPLE__)
        VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    #endif
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value();
        }
    };

    static inline QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            device, &queueFamilyCount, queueFamilies.data());

        for(uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
                break;
            }
        }
        return indices;
    }

    static inline uint32_t findMemoryType(Device* device, uint32_t type_filter, VkMemoryPropertyFlags properties) 
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(device->self->p_adapter, &mem_properties);
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

}