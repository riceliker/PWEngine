#include "render.hpp"

namespace PWEngine::Render 
{
    struct FrameSubmitCommand::Impl
    {
        VkCommandBuffer* command_buffer;
        VkExtent2D* swapchain_extent;
    };
}