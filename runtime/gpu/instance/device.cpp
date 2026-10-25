#include "_instance.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <set>

namespace PWEngine::GPU
{
    const std::vector<const char*> device_extensions = {
    #if (__APPLE__)
        VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    #endif
        /* swapchain */
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        /* dynamic rendering */
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    inline bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    void GPUInstance::createDevice()
    {
        /* get best adapter */
        uint32_t adapter_count = 0;
        vkEnumeratePhysicalDevices(this->self->instance, &adapter_count, nullptr);

        if (adapter_count == 0)
            sendLog(this->log, LogType::Error, LogFrom::VulkanRender, "failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> adapters(adapter_count);
        vkEnumeratePhysicalDevices(this->self->instance, &adapter_count, adapters.data());

        VkPhysicalDevice adapter = adapters[0];
        VkDevice device;
        VkQueue graphics_queue;
        QueueFamilyIndices indices = findQueueFamilies(adapter);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        
        float queue_priority = 1.0f;
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = indices.graphicsFamily.value();
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);

        VkPhysicalDeviceFeatures physics_features{};
        vkGetPhysicalDeviceFeatures(adapter, &physics_features);

        VkPhysicalDeviceFeatures need_features{};
        if (physics_features.samplerAnisotropy) need_features.samplerAnisotropy = VK_TRUE;
        

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pQueueCreateInfos = queue_create_infos.data();

        device_create_info.pEnabledFeatures = &need_features;

        device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();

        const VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feature
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = nullptr,
            .dynamicRendering = VK_TRUE,
        };
        device_create_info.pNext = &dynamic_rendering_feature;

        if (vkCreateDevice(adapter, &device_create_info, nullptr, &device) != VK_SUCCESS)
            sendLog(this->log, LogType::Error, LogFrom::VulkanRender, "failed to create logical device!");

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphics_queue);

        this->self->adapter = adapter;
        this->self->device = device;
        this->self->graphics_queue = graphics_queue;
    }
}