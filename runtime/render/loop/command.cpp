#include "render.hpp"
#include "./render/_render.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace PWEngine::Render
{
    Command::Command() :self(std::make_unique<Impl>())
    {
        
    }
    /*
        Create the command buffer
    */
    Command* RenderContext::commandBegin(size_t swapchain_loop_frame_index, uint32_t swapchain_image_index)
    {
        Command* obj = new Command();
        obj->p_context = this;
        obj->self->extent = this->m_swapchain->swapchain_extent;
        obj->swapchain_loop_frame_index = swapchain_loop_frame_index;
        obj->swapchain_image_index = swapchain_image_index;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        obj->self->command_buffer = &this->self->command_buffers[swapchain_loop_frame_index];

        if (vkBeginCommandBuffer(*obj->self->command_buffer, &beginInfo) != VK_SUCCESS) {
            Stream::log(this->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to begin recording command buffer!");
        }
        
        return obj;
    }

    void RenderContext::commandEnd(Command* command)
    {
        if (vkEndCommandBuffer(*command->self->command_buffer) != VK_SUCCESS) {
            Stream::log(this->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to record command buffer!");
        }
        delete command;
    }

    void Command::setViewPort()
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) this->self->extent.width;
        viewport.height = (float) this->self->extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(*this->self->command_buffer, 0, 1, &viewport);
    }

    void Command::setScissor()
    {
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = this->self->extent;
        vkCmdSetScissor(*this->self->command_buffer, 0, 1, &scissor);            
    }

    void Command::setPipeline(Pipeline3D* pipeline)
    {
        vkCmdBindPipeline(*this->self->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
    }
}