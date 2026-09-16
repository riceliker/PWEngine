#pragma once
#include "render.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace PWEngine::Render 
{
    struct FrameCommandRendering::Impl
    {
        VkCommandBuffer command_buffer;
        VkExtent2D swapchain_extent;
        VkPipelineLayout pipeline_layout;
        std::vector<std::vector<VkDescriptorSet>> descriptor_sets;
    };

    void singleTimerCommand(RenderContext* context, std::function<void(VkCommandBuffer& cmd)> func);
    void copyBufferCommand(RenderContext* context, VkBuffer staging, VkBuffer real, size_t size);
    void copyBufferToImage(RenderContext* context, VkBuffer buffer, VkImage image, Utils::Vec2<uint32_t> size);
    void transitionImageLayout(RenderContext* context, uint32_t min_level, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void generateMipmap(RenderContext* context, uint32_t mip_level, Utils::Vec2<uint32_t> size, VkImage image);
}