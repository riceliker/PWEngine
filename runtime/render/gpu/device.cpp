#include "checker.hpp"
#include "render.hpp"
#include <set>

namespace PWEngine::Render::GPU
{

    struct swapchain_supportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(device_extensions.begin(),
                                                 device_extensions.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    swapchain_supportDetails queryswapchain_support(VkPhysicalDevice device,
                                                  VkSurfaceKHR surface)
    {
        swapchain_supportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                device, surface, &formatCount, details.formats.data());
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, surface, &present_mode_count, nullptr);

        if (present_mode_count != 0)
        {
            details.presentModes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                surface,
                &present_mode_count,
                details.presentModes.data());
        }

        return details;
    }

    static inline bool isDeviceSuitable(VkPhysicalDevice device,
                                        VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = findQueueFamilies(device, surface);

        bool extensions_supported = checkDeviceExtensionSupport(device);

        bool swap_chain_adequate = false;
        if (extensions_supported)
        {
            swapchain_supportDetails swap_chain_support =
                queryswapchain_support(device, surface);
            swap_chain_adequate = !swap_chain_support.formats.empty() &&
                                  !swap_chain_support.presentModes.empty();
        }

        return indices.isComplete() && extensions_supported &&
               swap_chain_adequate;
    }

    /*
        ██░ ▓███    ████ ▒███████  ██▒
        ██░ ▓████  █████ ▒██   ▒██ ██▒
        ██░ ▓██▒██▓█▒███ ▒███████  ██▒
        ██░ ▓██ ████ ███ ▒██       ██▒
        ██░ ▓██  ██  ███ ▒██       ███████▒
    */

    std::optional<Device> Instance::GetBestDevice(Window window)
    {
        if (window.surface.has_value())
        {

        }
        VkPhysicalDevice adapter = VK_NULL_HANDLE;
        for (const auto& device : devices)
        {
            if (isDeviceSuitable(device, window.surface.value()))
            {
                adapter = device;
                break;
            }
        }
        if (adapter == VK_NULL_HANDLE)
            return std::nullopt;

        VkDevice device;
        VkQueue graphics_queue;
        VkQueue present_queue;
        QueueFamilyIndices indices =
            findQueueFamilies(adapter, window.surface.value());

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {
            indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType =
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queueFamily;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queuePriority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features{};

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        device_create_info.queueCreateInfoCount =
            static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pQueueCreateInfos = queue_create_infos.data();

        device_create_info.pEnabledFeatures = &device_features;

        device_create_info.enabledExtensionCount =
            static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();

        if (vkCreateDevice(
                adapter, &device_create_info, nullptr, &device) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(
            device, indices.graphicsFamily.value(), 0, &graphics_queue);
        vkGetDeviceQueue(
            device, indices.presentFamily.value(), 0, &present_queue);

        Device self;
        self.ptr = device;
        self.adapter = adapter;
        self.graphics_queue = graphics_queue;
        self.present_queue = present_queue;
        return self;
    }

    void Device::waitIdle()
    {
        vkDeviceWaitIdle(this->ptr);
    }
} // namespace PWEngine::Render::GPU