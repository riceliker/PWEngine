#pragma once
#include "render.hpp"

namespace PWEngine::Render
{
    struct RenderInstance::Impl
    {
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkPhysicalDevice adapters;
        std::vector<RenderContext*> context_list;
    };
}