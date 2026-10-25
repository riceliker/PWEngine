#include "gpu.hpp"
#include "_window.hpp"
#include "gpu/allocator/_allocator.hpp"
#include "gpu/instance/_instance.hpp"
#include <cstdint>
#include <memory>

namespace PWEngine::GPU
{
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    static inline SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapchainSupportDetails details;

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
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.presentModes.data());
        }

        return details;
    }

    static inline VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void Window::createSwapchain(Vec2<uint32_t>& swapchain_extent)
    {
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        
        SwapchainSupportDetails swapchain_support = querySwapchainSupport(p_instance->self->adapter, this->self->surface);

        VkSurfaceFormatKHR available_format;
        available_format.format = VK_FORMAT_B8G8R8A8_SRGB;
        available_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        VkSurfaceFormatKHR surface_format = available_format;
        //VkPresentModeKHR present_mode = chooseSwapPresentMode(swapchain_support.presentModes);
        VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;

        VkExtent2D extent = chooseSwapExtent(this->self->window, swapchain_support.capabilities);

        uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
        if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount)
            image_count = swapchain_support.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = this->self->surface;

        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(this->p_instance->self->adapter);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value()};

        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        create_info.preTransform =
            swapchain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;

        create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(this->p_instance->self->device, &create_info, nullptr, &swapchain) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create swap chain!");

        vkGetSwapchainImagesKHR(this->p_instance->self->device, swapchain, &image_count, nullptr);
        swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(this->p_instance->self->device, swapchain, &image_count, swapchain_images.data());

        this->self->swapchain = swapchain;
        this->self->swapchain_image_format = surface_format.format;;
        this->self->swapchain_images = swapchain_images;
        swapchain_extent.x = extent.width;
        swapchain_extent.y = extent.height;
    }

    void Window::createDepth(Vec2<uint32_t> swapchain_extent)
    {
        VkImageView depth_image_view;

        CreateImageInfo depth_info = {
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            {swapchain_extent.x, swapchain_extent.y},
            1,
            ImageFormat::D_F32_S_U8
        };
        auto [depth_image, depth_image_memory] = createImage(this->p_instance, depth_info).value();

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depth_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(this->p_instance->self->device, &viewInfo, nullptr, &depth_image_view) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create depth image view!");

        this->self->depth_image = depth_image;
        this->self->depth_image_memory = depth_image_memory;
        this->self->depth_image_view = depth_image_view;
        this->self->swapchain_extent = {swapchain_extent.x, swapchain_extent.y};
    }

    void Window::createImageView()
    {
        std::vector<VkImageView> swapchain_image_views;
        swapchain_image_views.resize(this->self->swapchain_images.size());

        for (size_t i = 0; i < this->self->swapchain_images.size(); i++) 
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = this->self->swapchain_images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = this->self->swapchain_image_format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(this->p_instance->self->device, &create_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS) 
                sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create image views!");  
        }
        this->self->swapchain_image_views = std::move(swapchain_image_views);
    }

    void Window::recreateSwapchain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(this->self->window, &width, &height);
        while (width == 0 || height == 0) 
        {
            glfwGetFramebufferSize(this->self->window, &width, &height);
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(this->p_instance->self->device);

        for (auto imageView : this->self->swapchain_image_views) 
        {
            vkDestroyImageView(this->p_instance->self->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(this->p_instance->self->device, this->self->swapchain, nullptr);

        Vec2<uint32_t> swapchain_extent;
        this->createSwapchain(swapchain_extent);
        this->createDepth(swapchain_extent);
        this->createImageView();
    }
}