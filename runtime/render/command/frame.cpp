#include "render.hpp"
#include "impl.hpp"
#include "../context/impl.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    FrameCommandRendering::FrameCommandRendering(RenderContext* context): self(std::make_unique<Impl>()), p_context(context)
    {
    }

    void RenderContext::__waitFence()
    {
        vkWaitForFences(this->m_device->device, 1, &this->self->in_flight_fences[this->current_frame], VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(this->m_device->device, this->m_swapchain->swapchain, UINT64_MAX, this->self->image_available_semaphores[current_frame], VK_NULL_HANDLE, &this->image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            this->recreateSwapchain();
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(this->m_device->device, 1, &this->self->in_flight_fences[this->current_frame]);
        vkResetCommandBuffer(this->self->command_buffers[this->current_frame], /*VkCommandBufferResetFlagBits*/ 0);

    }

    FrameCommandFactory* RenderContext::__frameCommandStart()
    {
        auto factory = new FrameCommandFactory(this);
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(this->self->command_buffers[this->current_frame], &beginInfo) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to begin recording command buffer!");
        }

        return factory;
    }

    void RenderContext::__frameCommandEnd(FrameCommandFactory* factory)
    {
        if (vkEndCommandBuffer(this->self->command_buffers[this->current_frame]) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to record command buffer!");
        }
        delete factory;
    }

    void RenderContext::frameSubmit()
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {this->self->image_available_semaphores[this->current_frame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->self->command_buffers[this->current_frame];

        VkSemaphore signalSemaphores[] = {this->self->render_finished_semaphores[this->current_frame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(this->m_device->graphics_queue, 1, &submitInfo, this->self->in_flight_fences[this->current_frame]) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {this->m_swapchain->swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &this->image_index;

        vkQueuePresentKHR(this->m_device->graphics_queue, &presentInfo);
    }

    FrameCommandRendering* FrameCommandFactory::__frameRenderingStart()
    {
        // Swapchain Color Image barrier
        VkImageMemoryBarrier color_barrier{};
        color_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        color_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        color_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        color_barrier.image = this->p_context->m_swapchain->swapchain_images[this->p_context->image_index];
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
        depth_barrier.image = this->p_context->m_swapchain->depth_image;
        depth_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT; /**/
        depth_barrier.subresourceRange.baseMipLevel = 0;
        depth_barrier.subresourceRange.levelCount = 1;
        depth_barrier.subresourceRange.baseArrayLayer = 0;
        depth_barrier.subresourceRange.layerCount = 1;

        VkImageMemoryBarrier barriers[] = {color_barrier, depth_barrier};
        vkCmdPipelineBarrier(
            this->p_context->self->command_buffers[this->p_context->current_frame],
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0,
            0, nullptr,
            0, nullptr,
            2, barriers
        );
        
        auto cmd = new FrameCommandRendering(this->p_context);
        VkRenderingAttachmentInfo color_attachment_info{};
        color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment_info.imageView = this->p_context->m_swapchain->swapchain_image_views[this->p_context->image_index];
        color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue.color = {{0.1f,0.1f,0.15f,1.0f}};

        VkRenderingAttachmentInfo depth_attachment_info{};
        depth_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth_attachment_info.imageView = this->p_context->m_swapchain->depth_image_view;
        depth_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment_info.clearValue.depthStencil.depth = 1.0f;
        depth_attachment_info.clearValue.depthStencil.stencil = 0;

        VkRenderingInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea.offset = {0, 0};
        rendering_info.renderArea.extent = this->p_context->m_swapchain->swapchain_extent;
        rendering_info.layerCount = 1;
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = &color_attachment_info;
        rendering_info.pDepthAttachment = &depth_attachment_info;
        rendering_info.pStencilAttachment = VK_NULL_HANDLE;
        
        vkCmdBeginRendering(this->p_context->self->command_buffers[this->p_context->current_frame], &rendering_info);

        

        cmd->p_context = this->p_context;
        cmd->self->command_buffer = this->p_context->self->command_buffers[this->p_context->current_frame];
        cmd->self->swapchain_extent = this->p_context->m_swapchain->swapchain_extent;

        return cmd;
    }

    void FrameCommandFactory::__frameRenderingEnd(FrameCommandRendering* cmd)
    {
        vkCmdEndRendering(this->p_context->self->command_buffers[this->p_context->current_frame]);
        VkImageMemoryBarrier present_barrier{};
        present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        present_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        present_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        present_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        present_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        present_barrier.image = this->p_context->m_swapchain->swapchain_images[this->p_context->image_index];
        present_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        present_barrier.subresourceRange.baseMipLevel = 0;
        present_barrier.subresourceRange.levelCount = 1;
        present_barrier.subresourceRange.baseArrayLayer = 0;
        present_barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            this->p_context->self->command_buffers[this->p_context->current_frame],
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &present_barrier
        );
        delete cmd;
    }

    void FrameCommandRendering::setViewPort()
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) this->self->swapchain_extent.width;
        viewport.height = (float) this->self->swapchain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(this->self->command_buffer, 0, 1, &viewport);
    }

    void FrameCommandRendering::setScissor()
    {
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = this->self->swapchain_extent;
        vkCmdSetScissor(this->self->command_buffer, 0, 1, &scissor);            
    }

    void FrameCommandRendering::setPipeline(Pipeline3D* pipeline)
    {
        vkCmdBindPipeline(this->self->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
        this->self->pipeline_layout = pipeline->self->pipeline_layout;
    }

    void FrameCommandRendering::setDescriptorSet(Pipeline3D* pipeline)
    {
        std::vector<VkDescriptorSet> descriptor_sets_list;
        for (const auto descriptor_set : pipeline->self->descriptor_sets)
        {
            descriptor_sets_list.push_back(descriptor_set[this->p_context->image_index]);
        }
        vkCmdBindDescriptorSets(this->self->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
    }

}