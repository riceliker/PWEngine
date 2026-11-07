#include "_memory.hpp"
#include "gpu.hpp"
#include <memory>
namespace PWEngine::GPU 
{
    

    GPUImageView::GPUImageView(GPUImage* image) : self(std::make_unique<Impl>())
    {
        this->p_image = image;
        this->createTextureView();
        this->createTextureSampler();
    }

    GPUImageView::~GPUImageView()
    {
        vkDestroySampler(this->p_image->p_instance->self->device, this->self->sampler, nullptr);
        vkDestroyImageView(this->p_image->p_instance->self->device, this->self->view, nullptr);
    }

    void GPUImageView::createTextureView()
    {
        VkImageView texture_image_view;
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = this->p_image->self->image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = this->p_image->self->mip_level;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(this->p_image->p_instance->self->device, &viewInfo, nullptr, &texture_image_view) != VK_SUCCESS)
            sendLog(this->p_image->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create texture image view!");

        this->self->view = texture_image_view;
    }

    void GPUImageView::createTextureSampler()
    {
        VkSampler texture_sampler;
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(this->p_image->p_instance->self->adapter, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        if (vkCreateSampler(this->p_image->p_instance->self->device, &samplerInfo, nullptr, &texture_sampler) != VK_SUCCESS)
            sendLog(this->p_image->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create texture sampler!");

        this->self->sampler = texture_sampler;
    }

}