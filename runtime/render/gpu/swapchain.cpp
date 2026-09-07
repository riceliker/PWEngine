#include "render.hpp"
#include "impl.hpp"
#include "check.hpp"
#include "stream.hpp"
#include <cstddef>


namespace PWEngine::Render
{
    void RenderContext::createSwapchain(size_t render_pass_index)
    {
        VkRenderPass render_pass = this->self->render_passes.at(render_pass_index);
        /* swapchain */
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;
        std::vector<VkImageView> swapchain_image_views;
        std::vector<VkFramebuffer> swapchain_framebuffers;

        swapchain_supportDetails swapchain_support = querySwapchainSupport(this->self->adapter, this->self->surface);

        VkSurfaceFormatKHR available_format;
        available_format.format = VK_FORMAT_B8G8R8A8_SRGB;
        available_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        VkSurfaceFormatKHR surface_format = available_format;
        VkPresentModeKHR present_mode = chooseSwapPresentMode(swapchain_support.presentModes);
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

        QueueFamilyIndices indices = findQueueFamilies(this->self->adapter);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value()};

        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        create_info.preTransform =
            swapchain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;

        create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(this->self->device, &create_info, nullptr, &swapchain) != VK_SUCCESS)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create swap chain!");

        vkGetSwapchainImagesKHR(this->self->device, swapchain, &image_count, nullptr);
        swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(this->self->device, swapchain, &image_count, swapchain_images.data());

        swapchain_image_format = surface_format.format;
        swapchain_extent = extent;
        /* Image View */
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

            if (vkCreateImageView(this->self->device, &create_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS) {
                Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create image views!");
            }
        }

        /* framebuffer */
        
        swapchain_framebuffers.resize(swapchain_image_views.size());

        for (size_t i = 0; i < swapchain_image_views.size(); i++) {
            VkImageView attachments[] = {
                swapchain_image_views[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = render_pass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapchain_extent.width;
            framebufferInfo.height = swapchain_extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(this->self->device, &framebufferInfo, nullptr, &swapchain_framebuffers[i]) != VK_SUCCESS) {
                Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create framebuffer!");
            }
            
        }

        this->self->currect_render_pass_index = index;
        this->self->swapchain = swapchain;
        this->self->swapchain_extent = swapchain_extent;
        this->self->swapchain_image_format = swapchain_image_format;
        this->self->swapchain_images = swapchain_images;
        this->self->swapchain_image_views = std::move(swapchain_image_views);
        this->self->swapchain_framebuffers = std::move(swapchain_framebuffers);
    }

    void RenderContext::recreateSwapchain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(this->self->window, &width, &height);
        while (width == 0 || height == 0) 
        {
            glfwGetFramebufferSize(this->self->window, &width, &height);
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(this->self->device);

        for (auto framebuffer : this->self->swapchain_framebuffers) {
            vkDestroyFramebuffer(this->self->device, framebuffer, nullptr);
        }

        for (auto imageView : this->self->swapchain_image_views) {
            vkDestroyImageView(this->self->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(this->self->device, this->self->swapchain, nullptr);

        this->createSwapchain(this->self->currect_render_pass_index);
    }
} // namespace PWEngine::Render