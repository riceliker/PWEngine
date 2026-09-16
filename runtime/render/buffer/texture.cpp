#include "buffer.hpp"
#include "render.hpp"
#include "impl.hpp"
#include "../command/impl.hpp"
#include "utils.hpp"
#include <cstddef>
#include <memory>

namespace PWEngine::Render 
{
    void Mesh3D::setTexture2D(RenderContext* super, Utils::ImageRGBA8& image)
    {
        size_t size = image.size.x * image.size.y * image.depth / 8;
        this->m_texture->mip_level = std::log(std::max(image.size.x, image.size.y));
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        createStagingBuffer(super, size, staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(super->m_device->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, image.data.data(), size);
        vkUnmapMemory(super->m_device->device, staging_buffer_memory);

        VkImage texture_image;
        VkDeviceMemory texture_image_memory;

        createImageBuffer(super, image.size, this->m_texture->mip_level, texture_image, texture_image_memory);

        transitionImageLayout(super, this->m_texture->mip_level, texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(super, staging_buffer, texture_image, image.size);
        generateMipmap(super, this->m_texture->mip_level, image.size, texture_image);
        
        vkDestroyBuffer(super->m_device->device, staging_buffer, nullptr);
        vkFreeMemory(super->m_device->device, staging_buffer_memory, nullptr);

        this->m_texture->texture_image = texture_image;
        this->m_texture->texture_image_memory = staging_buffer_memory;
    }

    void Mesh3D::setTextureView(RenderContext* super)
    {
        VkImageView texture_image_view;
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = this->m_texture->texture_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = this->m_texture->mip_level;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(super->m_device->device, &viewInfo, nullptr, &texture_image_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
        this->m_texture->texture_image_view = texture_image_view;
    }

    void Mesh3D::setTextureSampler(RenderContext* super)
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
        vkGetPhysicalDeviceProperties(super->m_device->adapter, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        if (vkCreateSampler(super->m_device->device, &samplerInfo, nullptr, &texture_sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        this->m_texture->texture_sampler = texture_sampler;
    }

}