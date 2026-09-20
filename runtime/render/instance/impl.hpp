#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
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