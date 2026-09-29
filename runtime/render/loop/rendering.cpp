#include "render.hpp"
#include "./render/_render.hpp"
#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    void Command::renderingBegin(Vec4<float> color)
    {
        // Swapchain Color Image barrier
        VkImageMemoryBarrier color_barrier{};
        color_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        color_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        color_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        color_barrier.image = this->p_context->m_swapchain->swapchain_images[this->swapchain_image_index];
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
            this->p_context->self->command_buffers[this->swapchain_loop_frame_index],
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            0,
            0, nullptr,
            0, nullptr,
            2, barriers
        );
        
        VkRenderingAttachmentInfo color_attachment_info{};
        color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment_info.imageView = this->p_context->m_swapchain->swapchain_image_views[swapchain_image_index];
        color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue.color = {{color.x ,color.y ,color.z, color.w}};

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
        
        vkCmdBeginRendering(*this->self->command_buffer, &rendering_info);
    }

    void Command::renderingEnd()
    {
        vkCmdEndRendering(this->p_context->self->command_buffers[this->swapchain_loop_frame_index]);
        VkImageMemoryBarrier present_barrier{};
        present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        present_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        present_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        present_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        present_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        present_barrier.image = this->p_context->m_swapchain->swapchain_images[swapchain_image_index];
        present_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        present_barrier.subresourceRange.baseMipLevel = 0;
        present_barrier.subresourceRange.levelCount = 1;
        present_barrier.subresourceRange.baseArrayLayer = 0;
        present_barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            this->p_context->self->command_buffers[this->swapchain_loop_frame_index],
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &present_barrier
        );
    }

    void Command::draw(Pipeline3D* pipeline, Mesh3D* mesh, Material* material, Camera3D* camera)
    {
        std::vector<VkDescriptorSet> descriptor_sets_list = {
            mesh->m_descriptor_set->descriptor_sets.at(this->swapchain_loop_frame_index), 
            material->m_descriptor_set->descriptor_sets.at(this->swapchain_loop_frame_index),
            camera->m_descriptor_set->descriptor_sets.at(this->swapchain_loop_frame_index)
        };

        vkCmdBindDescriptorSets(*this->self->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
        VkBuffer vertex_buffers[] = {mesh->m_vertex->vertex_buffer};
        VkBuffer index_buffer = mesh->m_vertex->indices_buffer;
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(*this->self->command_buffer, 0, 1, vertex_buffers, offsets);
        vkCmdBindIndexBuffer(*this->self->command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(*this->self->command_buffer, mesh->indices_size, mesh->transform->length, 0, 0, 0);
    }
}