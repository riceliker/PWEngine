#include "render.hpp"
#include "impl.hpp"
#include "../gpu/impl.hpp"
#include "../buffer/impl.hpp"
#include <cstdint>
#include <functional>
#include <memory>

namespace PWEngine::Render 
{
    FrameSubmitCommand::FrameSubmitCommand(): self(std::make_unique<Impl>())
    {

    }
    
    void RenderContext::drawFrame(Pipeline* pipeline, std::function<void(FrameSubmitCommand& cmd)> func)
    {
        vkWaitForFences(this->self->device, 1, &this->self->in_flight_fences[this->current_frame], VK_TRUE, UINT64_MAX);

        uint32_t image_index;
        VkResult result = vkAcquireNextImageKHR(this->self->device, this->self->swapchain, UINT64_MAX, this->self->image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            this->recreateSwapchain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(this->self->device, 1, &this->self->in_flight_fences[this->current_frame]);
        vkResetCommandBuffer(this->self->command_buffers[this->current_frame], /*VkCommandBufferResetFlagBits*/ 0);

        /* submit */
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(this->self->command_buffers[this->current_frame], &beginInfo) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = pipeline->p_render_pass;
        render_pass_info.framebuffer = this->self->swapchain_framebuffers[image_index];
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = this->self->swapchain_extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clearColor;

        vkCmdBeginRenderPass(this->self->command_buffers[this->current_frame], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(this->self->command_buffers[this->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphics_pipeline);
        FrameSubmitCommand cmd{};
        cmd.self->command_buffer = &(this->self->command_buffers[this->current_frame]);
        cmd.self->swapchain_extent = &(this->self->swapchain_extent);
        func(cmd);

        vkCmdEndRenderPass(this->self->command_buffers[this->current_frame]);

        if (vkEndCommandBuffer(this->self->command_buffers[this->current_frame]) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to record command buffer!");
        }

        /* submit*/
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

        if (vkQueueSubmit(this->self->graphics_queue, 1, &submitInfo, this->self->in_flight_fences[this->current_frame]) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {this->self->swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &image_index;

        vkQueuePresentKHR(this->self->graphics_queue, &presentInfo);
    }

    void FrameSubmitCommand::setViewPort()
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) this->self->swapchain_extent->width;
        viewport.height = (float) this->self->swapchain_extent->height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(*this->self->command_buffer, 0, 1, &viewport);
    }

    void FrameSubmitCommand::setScissor()
    {
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = *this->self->swapchain_extent;
        vkCmdSetScissor(*this->self->command_buffer, 0, 1, &scissor);            
    }

    void FrameSubmitCommand::addVertexBuffer(VertexBuffer* vertex_buffer)
    {
        VkBuffer vertexBuffers[] = {vertex_buffer->self->vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(*this->self->command_buffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(*this->self->command_buffer, static_cast<uint32_t>(vertex_buffer->vertices.size()), 1, 0, 0);
    }
    
    // void CommandPool::createSimpleTimeCommand(std::function<void(SimpleTimeCommand cmd)> func)
    // {
    //     VkCommandBufferAllocateInfo allocInfo{};
    //     allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    //     allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    //     allocInfo.commandPool = this->ptr;
    //     allocInfo.commandBufferCount = 1;

    //     VkCommandBuffer commandBuffer;
    //     vkAllocateCommandBuffers(this->self->ptr, &allocInfo, &commandBuffer);

    //     VkCommandBufferBeginInfo beginInfo{};
    //     beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //     beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    //     vkBeginCommandBuffer(commandBuffer, &beginInfo);

    //     SimpleTimeCommand cmd;
    //     cmd.p_pool = this;
    //     cmd.ptr = commandBuffer;
    //     func(cmd);

    //     vkEndCommandBuffer(commandBuffer);

    //     VkSubmitInfo submitInfo{};
    //     submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //     submitInfo.commandBufferCount = 1;
    //     submitInfo.pCommandBuffers = &commandBuffer;

    //     vkQueueSubmit(this->p_device->self->graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    //     vkQueueWaitIdle(this->p_device->self->graphics_queue);

    //     vkFreeCommandBuffers(this->p_device->self->ptr, this->ptr, 1, &commandBuffer);
    // }

    // void CommandPool::copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
    // {
    //     this->createSimpleTimeCommand([&](CommandBuffer cmd){
    //         VkBufferCopy copy_region{};
    //         copy_region.size = size;
    //         vkCmdCopyBuffer(cmd.ptr, src_buffer, dst_buffer, 1, &copy_region);
    //     });
    // }

    // void CommandPool::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    // {
    //     this->createSimpleTimeCommand([&](CommandBuffer cmd){
    //         VkImageMemoryBarrier barrier{};
    //         barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //         barrier.oldLayout = oldLayout;
    //         barrier.newLayout = newLayout;
    //         barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //         barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //         barrier.image = image;
    //         barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //         barrier.subresourceRange.baseMipLevel = 0;
    //         barrier.subresourceRange.levelCount = 1;
    //         barrier.subresourceRange.baseArrayLayer = 0;
    //         barrier.subresourceRange.layerCount = 1;
    //         barrier.srcAccessMask = 0; // TODO
    //         barrier.dstAccessMask = 0; // TODO

    //         VkPipelineStageFlags sourceStage;
    //         VkPipelineStageFlags destinationStage;

    //         if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    //             barrier.srcAccessMask = 0;
    //             barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    //             sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    //             destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    //         } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    //             barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //             barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    //             sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    //             destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    //         } else {
    //             throw std::invalid_argument("unsupported layout transition!");
    //         }

    //         vkCmdPipelineBarrier(
    //             cmd.ptr,
    //             sourceStage, destinationStage,
    //             0,
    //             0, nullptr,
    //             0, nullptr,
    //             1, &barrier
    //         );
    //     });
        
    // }

    // void CommandPool::copyBufferToImage(VkBuffer buffer, Texture texture)
    // {
    //     this->createSimpleTimeCommand([&](CommandBuffer cmd){
    //         VkBufferImageCopy region{};
    //         region.bufferOffset = 0;
    //         region.bufferRowLength = 0;
    //         region.bufferImageHeight = 0;

    //         region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //         region.imageSubresource.mipLevel = 0;
    //         region.imageSubresource.baseArrayLayer = 0;
    //         region.imageSubresource.layerCount = 1;

    //         region.imageOffset = {0, 0, 0};
    //         region.imageExtent = {(uint32_t)texture.size.x,(uint32_t)texture.size.y,1};
    //         vkCmdCopyBufferToImage(
    //             cmd.ptr,
    //             buffer,
    //             texture.self->texture_image,
    //             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //             1,
    //             &region
    //         );

            
    //     });

        
    //}
}