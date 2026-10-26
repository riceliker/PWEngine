#include "gpu.hpp"
#include "gpu/task/_task.hpp"
#include "gpu/pipeline/_pipeline.hpp"
#include "gpu/window/_window.hpp"

namespace PWEngine::GPU 
{
    void RecordCommandWithWindow::bindPipeline(Pipeline *pipeline, PipelineType type)
    {
        switch (type) 
        {
        case PipelineType::Render:
            vkCmdBindPipeline(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
        break;
        case PipelineType::Compute:
            vkCmdBindPipeline(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
        break;
        }
    }

    void RecordCommandWithWindow::setViewPort()
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) this->p_task->m_window->self->swapchain_extent.width;
        viewport.height = (float) this->p_task->m_window->self->swapchain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(this->p_task->self->command_buffers[this->p_task->flight_fences_index], 0, 1, &viewport);
    }

    void RecordCommandWithWindow::setScissor()
    {
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = this->p_task->m_window->self->swapchain_extent;
        vkCmdSetScissor(this->p_task->self->command_buffers[this->p_task->flight_fences_index], 0, 1, &scissor);            
    }

    void RecordCommandWithWindow::BeginRendering(Vec4<float> color, bool is_clear_background)
    {
        // Swapchain Color Image barrier
        VkImageMemoryBarrier color_barrier{};
        color_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        color_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        color_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        color_barrier.image = this->p_task->m_window->self->swapchain_images[this->p_task->swapchain_image_index];
        color_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_barrier.subresourceRange.baseMipLevel = 0;
        color_barrier.subresourceRange.levelCount = 1;
        color_barrier.subresourceRange.baseArrayLayer = 0;
        color_barrier.subresourceRange.layerCount = 1;

        // Depth Image barrier
        VkImageMemoryBarrier depth_barrier{};
        depth_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        depth_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        depth_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        depth_barrier.image = this->p_task->m_window->self->depth_image;
        depth_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT; /**/
        depth_barrier.subresourceRange.baseMipLevel = 0;
        depth_barrier.subresourceRange.levelCount = 1;
        depth_barrier.subresourceRange.baseArrayLayer = 0;
        depth_barrier.subresourceRange.layerCount = 1;

        VkImageMemoryBarrier barriers[] = {color_barrier, depth_barrier};
        vkCmdPipelineBarrier(
            this->p_task->self->command_buffers[this->p_task->flight_fences_index],
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0,
            0, nullptr,
            0, nullptr,
            2, barriers
        );
        
        VkRenderingAttachmentInfo color_attachment_info{};
        color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment_info.imageView = this->p_task->m_window->self->swapchain_image_views[this->p_task->swapchain_image_index];
        color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.loadOp = (is_clear_background)? VK_ATTACHMENT_LOAD_OP_CLEAR: VK_ATTACHMENT_LOAD_OP_LOAD;
        color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue.color = {{color.x ,color.y ,color.z, color.w}};

        VkRenderingAttachmentInfo depth_attachment_info{};
        depth_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth_attachment_info.imageView = this->p_task->m_window->self->depth_image_view;
        depth_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment_info.clearValue.depthStencil.depth = 1.0f;
        depth_attachment_info.clearValue.depthStencil.stencil = 0;

        VkRenderingInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea.offset = {0, 0};
        rendering_info.renderArea.extent = this->p_task->m_window->self->swapchain_extent;
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment_info;
        rendering_info.pDepthAttachment = &depth_attachment_info;
        rendering_info.pStencilAttachment = VK_NULL_HANDLE;
        
        vkCmdBeginRendering(this->p_task->self->command_buffers[this->p_task->flight_fences_index], &rendering_info);
    }

    void RecordCommandWithWindow::EndRendering()
    {
        vkCmdEndRendering(this->p_task->self->command_buffers[this->p_task->flight_fences_index]);
        VkImageMemoryBarrier present_barrier{};
        present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        present_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        present_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        present_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        present_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        present_barrier.image = this->p_task->m_window->self->swapchain_images[this->p_task->swapchain_image_index];
        present_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        present_barrier.subresourceRange.baseMipLevel = 0;
        present_barrier.subresourceRange.levelCount = 1;
        present_barrier.subresourceRange.baseArrayLayer = 0;
        present_barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            this->p_task->self->command_buffers[this->p_task->flight_fences_index],
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &present_barrier
        );
    }
}