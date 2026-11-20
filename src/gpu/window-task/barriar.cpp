#include "gpu.hpp"
#include "_task.hpp"
#include <memory>

namespace PWEngine::GPU
{
    void WindowRecordCommand::renderingImageBarrier(ResultImage* image)
    {
        VkImageMemoryBarrier image_barrier{};
        image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

        image_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        image_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        image_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        image_barrier.image = image->self->result_image;

        image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_barrier.subresourceRange.baseMipLevel = 0;
        image_barrier.subresourceRange.levelCount = 1;
        image_barrier.subresourceRange.baseArrayLayer = 0;
        image_barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            this->p_task->self->command_buffers.at(this->p_task->flight_fences_index),
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &image_barrier
        );
    }
}