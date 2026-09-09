#include "buffer.hpp"
#include "render.hpp"
#include "impl.hpp"
#include "../command/impl.hpp"
#include "stream.hpp"
#include <cstddef>
#include <memory>

namespace PWEngine::Render 
{
    void Texture2D::createTexture(Utils::Image* const image)
    {
        size_t size = image->size.x * image->size.y * image->depth;
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        createStagingBuffer(this->p_context, size, staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(this->p_context->self->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, image->data.data(), size);
        vkUnmapMemory(this->p_context->self->device, staging_buffer_memory);

        VkImage texture_image;
        VkDeviceMemory texture_image_memory;

        /* VkImage Buffer*/
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = static_cast<uint32_t>(image->size.x);
        image_info.extent.height = static_cast<uint32_t>(image->size.y);
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = VK_FORMAT_R8G8B8A8_SRGB; /* RGBA8 */
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional

        if (vkCreateImage(this->p_context->self->device, &image_info, nullptr, &texture_image) != VK_SUCCESS) 
        {
            Stream::log(this->p_context->log, Stream::LogType::Warn, Stream::LogFrom::VulkanRender, "failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(this->p_context->self->device, texture_image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(this->p_context, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(this->p_context->self->device, &allocInfo, nullptr, &texture_image_memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(this->p_context->self->device, texture_image, texture_image_memory, 0);

        transitionImageLayout(this->p_context, texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(this->p_context, staging_buffer, texture_image, image->size);
        transitionImageLayout(this->p_context, texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(this->p_context->self->device, staging_buffer, nullptr);
        vkFreeMemory(this->p_context->self->device, staging_buffer_memory, nullptr);

        this->self->texture_image = texture_image;
        this->self->texture_image_memory = staging_buffer_memory;
    }

    void Texture2D::createView()
    {
        VkImageView texture_image_view;
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = this->self->texture_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(this->p_context->self->device, &viewInfo, nullptr, &texture_image_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
        this->self->texture_image_view = texture_image_view;
    }

    void Texture2D::createSampler()
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
        vkGetPhysicalDeviceProperties(this->p_context->self->adapter, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        if (vkCreateSampler(this->p_context->self->device, &samplerInfo, nullptr, &texture_sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        this->self->texture_sampler = texture_sampler;
    }

    // void createDescriptorSetLayout() {
    //     VkDescriptorSetLayoutBinding uboLayoutBinding{};
    //     uboLayoutBinding.binding = 0;
    //     uboLayoutBinding.descriptorCount = 1;
    //     uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //     uboLayoutBinding.pImmutableSamplers = nullptr;
    //     uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    //     VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    //     samplerLayoutBinding.binding = 1;
    //     samplerLayoutBinding.descriptorCount = 1;
    //     samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //     samplerLayoutBinding.pImmutableSamplers = nullptr;
    //     samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    //     std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
    //     VkDescriptorSetLayoutCreateInfo layoutInfo{};
    //     layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //     layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    //     layoutInfo.pBindings = bindings.data();

    //     if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
    //         throw std::runtime_error("failed to create descriptor set layout!");
    //     }
    // }

    // void createDescriptorSets() {
    //     std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    //     VkDescriptorSetAllocateInfo allocInfo{};
    //     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    //     allocInfo.descriptorPool = descriptorPool;
    //     allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    //     allocInfo.pSetLayouts = layouts.data();

    //     descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    //     if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
    //         throw std::runtime_error("failed to allocate descriptor sets!");
    //     }

    //     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    //         VkDescriptorBufferInfo bufferInfo{};
    //         bufferInfo.buffer = uniformBuffers[i];
    //         bufferInfo.offset = 0;
    //         bufferInfo.range = sizeof(UniformBufferObject);

    //         VkDescriptorImageInfo imageInfo{};
    //         imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //         imageInfo.imageView = textureImageView;
    //         imageInfo.sampler = textureSampler;

    //         std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    //         descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //         descriptorWrites[0].dstSet = descriptorSets[i];
    //         descriptorWrites[0].dstBinding = 0;
    //         descriptorWrites[0].dstArrayElement = 0;
    //         descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //         descriptorWrites[0].descriptorCount = 1;
    //         descriptorWrites[0].pBufferInfo = &bufferInfo;

    //         descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //         descriptorWrites[1].dstSet = descriptorSets[i];
    //         descriptorWrites[1].dstBinding = 1;
    //         descriptorWrites[1].dstArrayElement = 0;
    //         descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //         descriptorWrites[1].descriptorCount = 1;
    //         descriptorWrites[1].pImageInfo = &imageInfo;

    //         vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    //     }
    // }

    std::unique_ptr<Texture2D> RenderContext::createTexture2D(Utils::Image* const image)
    {
        
        std::unique_ptr<Texture2D> obj;
        obj->p_context = this;
        obj->size = image->size;
        obj->createTexture(image);
        obj->createView();
        obj->createSampler();
        return obj;
    }

    Texture2D::~Texture2D()
    {
        vkDestroySampler(this->p_context->self->device, this->self->texture_sampler, nullptr);
        vkDestroyImageView(this->p_context->self->device, this->self->texture_image_view, nullptr);
        vkDestroyImage(this->p_context->self->device, this->self->texture_image, nullptr);
        vkFreeMemory(this->p_context->self->device, this->self->texture_image_memory, nullptr);
    }
}