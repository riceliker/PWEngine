// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#pragma once
/* include library */
#include "gpu.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
/* std library */
#include <cstddef>

namespace PWEngine::GPU
{
    struct Window::Impl
    {
        GLFWwindow* window;
        VkSurfaceKHR surface;

        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;

        VkImage depth_image;
        VkDeviceMemory depth_image_memory;
        VkImageView depth_image_view;

        std::vector<VkImageView> swapchain_image_views;
    };

    struct WindowTask::Impl
    {
        std::vector<VkSemaphore> image_available_semaphores;
        std::vector<VkSemaphore> render_finished_semaphores;
        std::vector<VkFence> in_flight_fences;

        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;
    };
}