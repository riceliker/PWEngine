#include "_memory.hpp"
#include "gpu.hpp"

namespace PWEngine::GPU
{
    IGPUBuffer::IGPUBuffer(): self(std::make_unique<Impl>())
    {

    }

    VkMemoryAllocateInfo getBufferAllocatorInfo(GPUInstance* instance, VkBuffer& buffer, VkMemoryPropertyFlags properties)
    {
        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(instance->self->device, buffer, &mem_requirements);

        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(instance->self->adapter, &mem_properties);
        uint32_t index;
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i) 
        {
            if ((mem_requirements.memoryTypeBits & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                VkMemoryAllocateInfo alloc_info{};
                alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc_info.allocationSize = mem_requirements.size;
                alloc_info.memoryTypeIndex = index;
                return alloc_info;
            }
        }      
        sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to find suitable memory type! memoryTypeBits="+std::to_string(mem_requirements.memoryTypeBits));
        return {};
    }

    VkMemoryAllocateInfo getImageAllocatorInfo(GPUInstance* instance, VkImage& image, VkMemoryPropertyFlags properties)
    {
        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(instance->self->device, image, &mem_requirements);

        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(instance->self->adapter, &mem_properties);
        uint32_t index;
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i) 
        {
            if ((mem_requirements.memoryTypeBits & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                VkMemoryAllocateInfo alloc_info{};
                alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                alloc_info.allocationSize = mem_requirements.size;
                alloc_info.memoryTypeIndex = index;
                return alloc_info;
            }
        }      
        sendLog(instance->log, LogType::Error, LogFrom::VulkanRender, "failed to find suitable memory type! memoryTypeBits="+std::to_string(mem_requirements.memoryTypeBits));
        return {};
    }

    VkBufferUsageFlags getBufferUsage(BufferUsageMask usage)
    {
        VkBufferUsageFlags result = 0;
        if (usage.test(PWEngine::GPU::BufferUsage::copy_dst)) result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (usage.test(PWEngine::GPU::BufferUsage::copy_src)) result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (usage.test(PWEngine::GPU::BufferUsage::vertex)) result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (usage.test(PWEngine::GPU::BufferUsage::index)) result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (usage.test(PWEngine::GPU::BufferUsage::uniform)) result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (usage.test(PWEngine::GPU::BufferUsage::storage)) result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        return result;
    } 

    template<std::invocable<VkCommandBuffer&> F> inline void instanceCommand(GPUInstance* instance, F&& func)
    {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = instance->self->command_pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(instance->self->device, &alloc_info, &command_buffer);
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
        func(command_buffer);
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(instance->self->graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(instance->self->graphics_queue);

        vkFreeCommandBuffers(instance->self->device, instance->self->command_pool, 1, &command_buffer);
    
    }

    void copyBufferCommand(GPUInstance* instance, VkBuffer staging, VkBuffer real, VkBufferCopy copy_region)
    {
        instanceCommand(instance, [&](VkCommandBuffer& command_buffer){
            vkCmdCopyBuffer(command_buffer, staging, real, 1, &copy_region);
        });
    }

    void copyBufferToImageCommand(GPUInstance* instance, VkBuffer buffer, VkImage image, Vec2<uint32_t> size) 
    {
        instanceCommand(instance, [&](VkCommandBuffer& command_buffer){
            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = {0, 0, 0};
            region.imageExtent = {size.x,size.y,1};
            vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        });
    }

    void transitionImageLayoutCommand(GPUInstance* instance, uint32_t mip_level, VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
    {
        instanceCommand(instance, [&](VkCommandBuffer& command_buffer){
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = old_layout;
            barrier.newLayout = new_layout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mip_level;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = 0; // TODO
            barrier.dstAccessMask = 0; // TODO

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else {
                return ;
            }

            vkCmdPipelineBarrier(command_buffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        });
    }

    void generateMipmapCommand(GPUInstance* instance, uint32_t mip_level, Vec2<uint32_t> size, VkImage image)
    {
        instanceCommand(instance, [&](VkCommandBuffer& command_buffer){
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;

            int32_t mip_w = size.x;
            int32_t mip_h = size.y;
            for (uint32_t i = 1; i < mip_level; ++i)
            {
                barrier.subresourceRange.baseMipLevel = i - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                VkImageBlit blit{};
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mip_w, mip_h, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { mip_w > 1 ? mip_h / 2 : 1, mip_h > 1 ? mip_h / 2 : 1, 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                vkCmdBlitImage(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

                if (mip_w > 1) mip_w /= 2;
                if (mip_h > 1) mip_h /= 2;
            }
            barrier.subresourceRange.baseMipLevel = mip_level - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        });
    }
}