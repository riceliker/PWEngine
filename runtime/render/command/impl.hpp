#pragma once
#include "render.hpp"
#include <cstddef>
#include <cstdint>

namespace PWEngine::Render 
{
    struct FrameSubmitCommand::Impl
    {
        size_t currect_image;
        VkCommandBuffer* command_buffer;
        VkExtent2D* swapchain_extent;
        VkPipelineLayout pipeline_layout;
    };

    void singleTimerCommand(RenderContext* context, std::function<void(VkCommandBuffer& cmd)> func);

    void copyBufferCommand(RenderContext* context, VkBuffer staging, VkBuffer real, size_t size);
    void copyBufferToImage(RenderContext* context, VkBuffer buffer, VkImage image, Utils::Vec2<uint32_t> size);
    void transitionImageLayout(RenderContext* context, uint32_t min_level, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void generateMipmap(RenderContext* context, uint32_t mip_level, Utils::Vec2<uint32_t> size, VkImage image);
}