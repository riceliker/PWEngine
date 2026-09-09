#pragma once
#include "render.hpp"
#include <cstddef>
#include <vector>

namespace PWEngine::Render
{
    struct Instance::Impl
    {
        bool is_debug = false;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        std::vector<VkPhysicalDevice> adapters;
         
    };

    struct RenderContext::Impl
    {
        VkPhysicalDevice adapter;
        VkDevice device;
        VkQueue graphics_queue;
        GLFWwindow* window;
        VkSurfaceKHR surface;

        std::vector<VkRenderPass> render_passes;
        size_t currect_render_pass_index;
        std::vector<UBO> UBOs;

        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;
        std::vector<VkImageView> swapchain_image_views;
        std::vector<VkFramebuffer> swapchain_framebuffers;

        std::vector<VkSemaphore> image_available_semaphores;
        std::vector<VkSemaphore> render_finished_semaphores;
        std::vector<VkFence> in_flight_fences;

        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;

        VkDescriptorPool descriptor_pool;
        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
        std::vector<std::vector<VkDescriptorSet>> descriptor_sets;
    };

    struct UBO::Impl
    {
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Pipeline::Impl
    {
        VkRenderPass p_render_pass;
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;
        size_t currect_descriptor_set_layout_index;
    };
}