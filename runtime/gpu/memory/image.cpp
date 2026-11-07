#include "_memory.hpp"
#include "gpu.hpp"
#include <memory>
namespace PWEngine::GPU 
{
    GPUImage::GPUImage(GPUInstance* instance, File::ImageRGBA8* image, bool is_SRGB, bool is_use_mipmap)
        : GPUImage(instance, {image->size, (is_SRGB)? ImageFormat::RGBA8_SRGB: ImageFormat::RGBA8_UNORM, image->depth, image->data.data()}, is_use_mipmap)
    {}

    GPUImage::GPUImage(GPUInstance* instance, ImageInfo image_info, bool is_use_mipmap) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;

        size_t size = image_info.size.x * image_info.size.y * image_info.depth / 8;
        if (is_use_mipmap)
        {
            uint32_t mip_level = log(std::max(image_info.size.x, image_info.size.y));
            this->self->mip_level = mip_level;
        } 
        else 
        {
            this->self->mip_level = 1;
        }

        /* staging buffer */
        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
 
        VkBufferCreateInfo staging_buffer_info{};
        staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        staging_buffer_info.size = size;
        staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(p_instance->self->device, &staging_buffer_info, nullptr, &staging_buffer) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        
        VkMemoryAllocateInfo staging_alloc_info = getBufferAllocatorInfo(p_instance, staging_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(p_instance->self->device, &staging_alloc_info, nullptr, &staging_memory) != VK_SUCCESS) 
            sendLog(p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");
        vkBindBufferMemory(p_instance->self->device, staging_buffer, staging_memory, 0);

        /* copy data */
        void* temp;
        vkMapMemory(p_instance->self->device, staging_memory, 0, size, 0, &temp);
        memcpy(temp, image_info.data, size);
        vkUnmapMemory(p_instance->self->device, staging_memory);


        VkImage image;
        VkDeviceMemory memory;

        VkImageCreateInfo image_create_info{};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = image_info.size.x;
        image_create_info.extent.height = image_info.size.y;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = this->self->mip_level;
        image_create_info.arrayLayers = 1;
        switch (image_info.format) 
        {
        case ImageFormat::RGBA8_SRGB:
            image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        break;
        case ImageFormat::RGB8_SRGB:
            image_create_info.format = VK_FORMAT_R8G8B8_SRGB;
        break;
        case ImageFormat::RGBA8_UNORM:
            image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::RGB8_UNORM:
            image_create_info.format = VK_FORMAT_R8G8B8_UNORM;
        case ImageFormat::D_F32_S_U8:
            image_create_info.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
        }
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.flags = 0; // Optional

        if (vkCreateImage(instance->self->device, &image_create_info, nullptr, &image) != VK_SUCCESS) 
            sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create image!");
        
        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(instance->self->device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info = getImageAllocatorInfo(instance, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        if (vkAllocateMemory(instance->self->device, &alloc_info, nullptr, &memory) != VK_SUCCESS) 
            sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate buffer memory!");

        vkBindImageMemory(instance->self->device, image, memory, 0);

        /* make image */
        transitionImageLayoutCommand(this->p_instance, this->self->mip_level, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImageCommand(this->p_instance, staging_buffer, image, image_info.size);
        if (is_use_mipmap)
            generateMipmapCommand(this->p_instance, this->self->mip_level, image_info.size, image);
        
        vkDestroyBuffer(this->p_instance->self->device, staging_buffer, nullptr);
        vkFreeMemory(this->p_instance->self->device, staging_memory, nullptr);

        this->self->image = image;
        this->self->memory = memory;
    }

    GPUImage::~GPUImage()
    {
        vkDestroyImage(this->p_instance->self->device, this->self->image, nullptr);
        vkFreeMemory(this->p_instance->self->device, this->self->memory, nullptr);
    }
}