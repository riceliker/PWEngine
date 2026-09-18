#pragma once
#include "render.hpp"
#include <cstddef>
#include <vector>

namespace PWEngine::Render
{
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

    struct Pipeline3D::Impl
    {
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;

        std::vector<std::vector<VkDescriptorSet>> descriptor_sets;
        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
    };
}