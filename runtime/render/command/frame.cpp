#include "render.hpp"
#include "impl.hpp"
#include "../gpu/impl.hpp"
#include "../buffer/impl.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>

namespace PWEngine::Render 
{
    FrameSubmitCommand::FrameSubmitCommand(): self(std::make_unique<Impl>())
    {

    }
    
    void RenderContext::drawFrameCommand(Pipeline* pipeline, size_t descriptor_set_index, std::function<void(FrameSubmitCommand& cmd)> func)
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
        render_pass_info.renderPass = pipeline->self->p_render_pass;
        render_pass_info.framebuffer = this->self->swapchain_framebuffers[image_index];
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = this->self->swapchain_extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clearColor;

        vkCmdBeginRenderPass(this->self->command_buffers[this->current_frame], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(this->self->command_buffers[this->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
        vkCmdBindDescriptorSets(this->self->command_buffers[this->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline_layout, 0, 1, &this->self->descriptor_sets[descriptor_set_index][this->current_frame], 0, nullptr);
        FrameSubmitCommand cmd{};
        cmd.p_context = this;
        cmd.self->currect_image = image_index;
        cmd.self->command_buffer = &(this->self->command_buffers[this->current_frame]);
        cmd.self->swapchain_extent = &(this->self->swapchain_extent);
        cmd.self->pipeline_layout = pipeline->self->pipeline_layout;
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
        
        this->current_frame = (this->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
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

    void FrameSubmitCommand::addMesh2D(Mesh2D* mesh)
    {
        VkBuffer vertex_buffers[] = {mesh->self->vertex_buffer};
        VkBuffer index_buffer = mesh->self->indices_buffer;
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(*this->self->command_buffer, 0, 1, vertex_buffers, offsets);
        vkCmdBindIndexBuffer(*this->self->command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(*this->self->command_buffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
    }

    void FrameSubmitCommand::updateUBO(UBO* ubo, float time)
    {
        ubo->data.model = Utils::rotate(Utils::Mat4(1.0f), time * Utils::deg2rad(90), Utils::Vec3(0.0f, 0.0f, 1.0f));
        ubo->data.view = Utils::look(Utils::Vec3<float>(0.0f, 2.0f, 2.0f), Utils::Vec3<float>(0.0f, 0.0f, 0.0f), Utils::Vec3<float>(0.0f, 1.0f, 0.0f));
        ubo->data.project = Utils::perspective(Utils::deg2rad(45), this->self->swapchain_extent->width / (float) this->self->swapchain_extent->height, 0.1f , 10.0f);
        memcpy(ubo->self->uniform_buffers_mapped[this->self->currect_image], &ubo->data, sizeof(UBO::UBOData));
    }  
}