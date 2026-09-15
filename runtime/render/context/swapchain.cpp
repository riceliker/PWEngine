#include "render.hpp"
#include "impl.hpp"
#include "../buffer/buffer.hpp"
#include "check.hpp"
#include "stream.hpp"
#include <cstddef>

namespace PWEngine::Render
{
    void RenderContext::Swapchain::createSwapchain(RenderContext* super, VkExtent2D& swapchain_extent)
    {
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        
        swapchain_supportDetails swapchain_support = querySwapchainSupport(super->m_device->adapter, super->m_window->surface);

        VkSurfaceFormatKHR available_format;
        available_format.format = VK_FORMAT_B8G8R8A8_SRGB;
        available_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        VkSurfaceFormatKHR surface_format = available_format;
        VkPresentModeKHR present_mode = chooseSwapPresentMode(swapchain_support.presentModes);
        VkExtent2D extent = chooseSwapExtent(super->m_window->window, swapchain_support.capabilities);

        uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
        if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount)
            image_count = swapchain_support.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = super->m_window->surface;

        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(super->m_device->adapter);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value()};

        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        create_info.preTransform =
            swapchain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;

        create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(super->m_device->device, &create_info, nullptr, &swapchain) != VK_SUCCESS)
            Stream::log(super->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create swap chain!");

        vkGetSwapchainImagesKHR(super->m_device->device, swapchain, &image_count, nullptr);
        swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(super->m_device->device, swapchain, &image_count, swapchain_images.data());

        swapchain_extent = extent;
        this->swapchain = swapchain;
        this->swapchain_image_format = surface_format.format;;
        this->swapchain_images = swapchain_images;
    }

    void RenderContext::Swapchain::createDepth(RenderContext* super, VkExtent2D swapchain_extent)
    {
        VkImage depth_image;
        VkDeviceMemory depth_image_memory;
        VkImageView depth_image_view;
        createDepthBuffer(super, swapchain_extent, depth_image, depth_image_memory);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depth_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(super->m_device->device, &viewInfo, nullptr, &depth_image_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create depth image view!");
        }

        this->depth_image = depth_image;
        this->depth_image_memory = depth_image_memory;
        this->depth_image_view = depth_image_view;
    }

    void RenderContext::Swapchain::createImageView(RenderContext* super)
    {
        std::vector<VkImageView> swapchain_image_views;
        swapchain_image_views.resize(swapchain_images.size());

        for (size_t i = 0; i < swapchain_images.size(); i++) {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = swapchain_images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = swapchain_image_format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(super->m_device->device, &create_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS) {
                Stream::log(super->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create image views!");
            }     
        }
        this->swapchain_image_views = std::move(swapchain_image_views);
    }
    
    RenderContext::Swapchain::Swapchain(RenderContext* super)
    {
        VkExtent2D swapchain_extent;
        this->createSwapchain(super, swapchain_extent);
        this->createDepth(super, swapchain_extent);
        this->createImageView(super);
        this->swapchain_extent = swapchain_extent;
    }

    void RenderContext::recreateSwapchain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(this->m_window->window, &width, &height);
        while (width == 0 || height == 0) 
        {
            glfwGetFramebufferSize(this->m_window->window, &width, &height);
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(this->m_device->device);

        for (auto imageView : this->m_swapchain->swapchain_image_views) {
            vkDestroyImageView(this->m_device->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(this->m_device->device, this->m_swapchain->swapchain, nullptr);

        VkExtent2D swapchain_extent;
        this->m_swapchain->createSwapchain(this, swapchain_extent);
        this->m_swapchain->createDepth(this, swapchain_extent);
        this->m_swapchain->createImageView(this);
        this->m_swapchain->swapchain_extent = swapchain_extent;
    }
} // namespace PWEngine::Render