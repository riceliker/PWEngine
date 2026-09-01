#pragma once
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif

#include <string>
#include <vector>
#include <optional>

#include "utils.hpp"

/*
----- GPU Module -----
The GPU module is control the vulkan instance with C++ class.
Use it is simple, you should not write the template code.

Window->Instance->Window::BindInstance->Device->Swapchain->Pipeline->Swapchain::BindPipeline

*/
namespace PWEngine::Render 
{
    class Window;
}

namespace PWEngine::Render::GPU
{
    class Instance;
    struct WindowInfo
    {
        Utils::Vec2<uint32_t> size;
        std::string title;
        bool is_resizable;
    };
    class Window
    {
    private:
        GLFWwindow* ptr;
        std::optional<VkSurfaceKHR> surface;
    public:
        Window(WindowInfo info);
        void bindInstance(Instance instance);
        bool isClosed();
        friend class Instance;
        friend class Swapchain;
        friend class CommandBuffer;
    };

    class Device
    {
    private:
        VkPhysicalDevice adapter;
        VkDevice ptr;
        VkQueue graphics_queue;
        VkQueue present_queue;
    public:
        void waitIdle();
        friend class Instance;
        friend class Pipeline;
        friend class Swapchain;
        friend class CommandBuffer;
        friend class Sync;
    };

    class Pipeline;
    class Sync;
    class CommandBuffer;

    class Swapchain
    {
    private:
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;
        std::vector<VkImageView> swapchain_image_views;
        std::optional<std::vector<VkFramebuffer>> swapchain_framebuffers;
    public:
        Swapchain(Device device, Window window);
        void bindPipeline(Device device, Pipeline pipeline);
        void submit(Device device, Pipeline pipeline, CommandBuffer command_buffer, Sync sync);
        friend class Pipeline;
        friend class CommandBuffer;
    };

    class Pipeline
    {
    private:
        VkRenderPass render_pass;
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;
    public:
        Pipeline(Device device, Swapchain swapchain);
        friend class Swapchain;
    };

    class CommandBuffer
    {
    private:
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
    public:
        CommandBuffer(Device device, Window window);
        friend class Swapchain;
    };

    class Sync
    {
    private:
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;
    public:
        Sync(Device device);
        void wait(Device device);
        friend class Swapchain;
    };

    struct InstanceInfo
    {
        std::string name;
        Utils::Vec3<uint8_t> version;
        bool is_debug;
    };
    class Instance
    {
    private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        std::vector<VkPhysicalDevice> devices;
    public:
        Instance(InstanceInfo info, Window window);
        std::optional<Device> GetBestDevice(Window window);
        friend class Window;
    };
}

namespace PWEngine::Render 
{
    
}
