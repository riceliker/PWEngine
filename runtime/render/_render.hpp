// This file is part of PWEngine.
// PWEngine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PWEngine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with PWEngine.  If not, see <https://www.gnu.org/licenses/>.
// Copyright (C) 2026 riceliker & all contributors

#pragma once
/* include library */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
/* header */
#include "render.hpp"
/* std library */
#include <cstddef>


namespace PWEngine::Render 
{
 /*
    ██  ░███    ███   ███████  ██████████  ████     ███    ░██    ███████    █████████
    ██  ░████   ███  ██    ███    ▓██      ████▓    ████░  ░██  ▓██▒    ██▓  ██
    ██  ░██ ██  ███  █████▓       ▓██     ██  ██    ██░███ ░██  ███          ████████
    ██  ░██  ██▓███     ░█████    ▓██    ███  ▓██   ██░ ███░██  ██▓          ██
    ██  ░██   █████ ███     ██    ▓██   ██████████  ██░  ▒████  ███     ██▓  ██
    ██  ░██    ▓███  ▓███████     ▓██  ░██      ███ ██░    ███    ███████    █████████
*/
    struct RenderInstance::Impl
    {
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkPhysicalDevice adapters;
        std::vector<RenderContext*> context_list;
    };
/*
     ███████     ███████░   ███░    ██ ▓█████████ █████████ ▓██▒   ███ █████████
    ███    ███  ███    ███  █████   ██     ██▒    ███         ███ ██▒     ███
   ███         ███      ██▒ ██████  ██     ██▒    ████████▒    ████       ███
   ███         ███      ██▒ ███ ▓██ ██     ██▒    ███          ████▒      ███
    ██▓    ███  ███    ███  ███   ████     ██▒    ███        ▒██░ ███     ███
     ███████     ███████▓   ███    ███     ██▒    █████████ ███    ███    ███
*/

    struct RenderContext::Device
    {
        VkPhysicalDevice adapter;
        VkDevice device;
        VkQueue graphics_queue;
        Device(RenderContext* super);
    };

    struct RenderContext::Window
    {
        GLFWwindow* window;
        VkSurfaceKHR surface;
        
        Window(RenderContext* super, bool is_resizable, Utils::Vec2<uint32_t> window_default_resolution, std::string window_title);
    };

    struct RenderContext::Swapchain
    {
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;

        VkImage depth_image;
        VkDeviceMemory depth_image_memory;
        VkImageView depth_image_view;

        std::vector<VkImageView> swapchain_image_views;

        Swapchain(RenderContext* super);
        void createSwapchain(RenderContext* super, VkExtent2D& swapchain_extent);
        void createDepth(RenderContext* super, VkExtent2D swapchain_extent);
        void createImageView(RenderContext* super);
    };

    struct RenderContext::Impl
    {
        std::vector<VkSemaphore> image_available_semaphores;
        std::vector<VkSemaphore> render_finished_semaphores;
        std::vector<VkFence> in_flight_fences;

        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;

        VkDescriptorPool descriptor_pool;
    };
/*
    ████████   ███  ████████   █████████  ███       ███  ███     ██  ░█████████
    ██    ▓██  ███  ███    ██▒ ███        ███       ███  █████   ██  ░██
    ██    ███  ███  ███    ██░ █████████  ███       ███  ██████  ██  ░████████
    ████████   ███  ████████   ███        ███       ███  ███ ▓██ ██  ░██
    ██         ███  ███        ███        ███       ███  ███   ████  ░██
    ██         ███  ███        █████████  █████████ ███  ███    ███  ░█████████
*/ 
    struct Pipeline3D::Impl
    {
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
    };

    struct Texture2D::Impl
    {
        uint32_t mip_level;
        VkImage texture_image;
        VkDeviceMemory texture_image_memory;
        VkImageView texture_image_view;
        VkSampler texture_sampler;
    };

    struct Mesh3D::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Mesh3D::Vertex3D
    {
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        VkBuffer indices_buffer;
        VkDeviceMemory indices_buffer_memory;
    };
   
    struct Material::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Material::Uniform
    {
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
    };

    struct Node3D::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Camera::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Camera::Uniform
    {
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
       
    };

    struct Command::Impl
    {
        VkCommandBuffer* command_buffer;
        VkExtent2D extent;
    };

    struct Uniform::Impl
    {
        RenderContext* p_context;
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
    };

}