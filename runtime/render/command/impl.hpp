#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
#include "render.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>

namespace PWEngine::Render 
{
    struct Command::Impl
    {
        VkCommandBuffer* command_buffer;
        VkExtent2D extent;
    };

    void singleTimerCommand(RenderContext* context, std::function<void(VkCommandBuffer& cmd)> func);
    void copyBufferCommand(RenderContext* context, VkBuffer staging, VkBuffer real, size_t size);
    void copyBufferToImage(RenderContext* context, VkBuffer buffer, VkImage image, Utils::Vec2<uint32_t> size);
    void transitionImageLayout(RenderContext* context, uint32_t min_level, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void generateMipmap(RenderContext* context, uint32_t mip_level, Utils::Vec2<uint32_t> size, VkImage image);
}