#include "buffer.hpp"
#include "render.hpp"
#include "impl.hpp"
#include "../command/impl.hpp"
#include <memory>

namespace PWEngine::Render 
{
    Texture2D::Texture2D() : self(std::make_unique<Impl>())
    {

    }

    std::shared_ptr<Texture2D> RenderContext::createTexture2D(Utils::ImageRGBA8* surface)
    {
        auto obj = std::make_shared<Texture2D>();
        obj->p_context = this;
        obj->setTexture2D(surface);
        obj->setTextureView();
        obj->setTextureSampler();
        return obj;
    }

    void Texture2D::setTexture2D(Utils::ImageRGBA8* image)
    {
        size_t size = image->size.x * image->size.y * image->depth / 8;
        this->self->mip_level = std::log(std::max(image->size.x, image->size.y));
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        createStagingBuffer(this->p_context, size, staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(this->p_context->m_device->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, image->data.data(), size);
        vkUnmapMemory(this->p_context->m_device->device, staging_buffer_memory);

        VkImage texture_image;
        VkDeviceMemory texture_image_memory;

        createImageBuffer(this->p_context, image->size, this->self->mip_level, texture_image, texture_image_memory);

        transitionImageLayout(this->p_context, this->self->mip_level, texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(this->p_context, staging_buffer, texture_image, image->size);
        generateMipmap(this->p_context, this->self->mip_level, image->size, texture_image);
        
        vkDestroyBuffer(this->p_context->m_device->device, staging_buffer, nullptr);
        vkFreeMemory(this->p_context->m_device->device, staging_buffer_memory, nullptr);

        this->self->texture_image = texture_image;
        this->self->texture_image_memory = staging_buffer_memory;
    }

    void Texture2D::setTextureView()
    {
        VkImageView texture_image_view;
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = this->self->texture_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = this->self->mip_level;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(this->p_context->m_device->device, &viewInfo, nullptr, &texture_image_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
        this->self->texture_image_view = texture_image_view;
    }

    void Texture2D::setTextureSampler()
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
        vkGetPhysicalDeviceProperties(this->p_context->m_device->adapter, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        if (vkCreateSampler(this->p_context->m_device->device, &samplerInfo, nullptr, &texture_sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        this->self->texture_sampler = texture_sampler;
    }

    Texture2D::~Texture2D()
    {
        vkDestroySampler(this->p_context->m_device->device, this->self->texture_sampler, nullptr);
        vkDestroyImageView(this->p_context->m_device->device, this->self->texture_image_view, nullptr);
        vkDestroyImage(this->p_context->m_device->device, this->self->texture_image, nullptr);
        vkFreeMemory(this->p_context->m_device->device, this->self->texture_image_memory, nullptr);
    }
}