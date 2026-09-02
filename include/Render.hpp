#pragma once
#include "stream.hpp"
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif

#include <string>
#include <vector>

#include "utils.hpp"

/*
----- GPU Module -----
The GPU module is control the vulkan instance with C++ class.
Use it is simple, you should not write the template code.

Instance->N * Device
Device->N * Window
Window->Swapchain

Window->Instance->Window::BindInstance->Device->Swapchain->Pipeline->Swapchain::BindPipeline

*/
namespace PWEngine::Render
{
    struct InstanceInfo
    {
        std::string name;
        Utils::Vec3<uint8_t> version;
        bool is_debug;
    };

    struct WindowInfo
    {
        Utils::Vec2<uint32_t> size;
        std::string title;
        bool is_resizable;
    };

    class Instance;
    class Device;
    class Window;
    class RenderPass;
    class Pipeline;
    class Swapchain;
    class CommandPool;
    class CommandBuffer;
    class Sync;

    class Instance
    {
    private:
        /* Log */
        Stream::LogSystem* log;
        /* owner */
        std::vector<Device*> devices;
        /* self */
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        std::vector<VkPhysicalDevice> adapters;
        void createInstance(InstanceInfo info);
        void getAllAdapter();
    public:
        Instance(InstanceInfo info, Stream::LogSystem* log);
        ~Instance();
        Device* GetBestDevice();
        Window* CreateWindow(WindowInfo info, Device* device);
    };

    class Device
    {
    private:
        /* owner */
        std::vector<Window*> windows;
        std::vector<RenderPass*> render_passes;
        std::vector<CommandPool*> command_pools;
        std::vector<Sync*> syncs;
        /* self */
        VkPhysicalDevice adapter;
        VkDevice ptr;
        VkQueue graphics_queue;
        void registryWindow();
        ~Device();
    public:
        RenderPass* createRenderPass();
        CommandPool* createCommandPool();
        Sync* createSync();
        void waitIdle();
        friend class Window;
        friend class Instance;
        friend class Pipeline;
        friend class Swapchain;
        friend class CommandBuffer;
        friend class Sync;
        friend class RenderPass;
        friend class CommandPool;
    };

    class Window
    {
    private:
        /* parent */
        Device* p_device;
        /* self */
        GLFWwindow* ptr;
        VkSurfaceKHR surface;
    public:
        Swapchain* createSwapchain(RenderPass* render_pass);
        bool isClosed();
        friend class Instance;
        friend class Swapchain;
        friend class CommandBuffer;
        friend class Device;
    };

    class RenderPass
    {
    private:
        /* owner */
        Device* p_device;
        /* self*/
        VkRenderPass ptr;
    public:
        Pipeline* createPipeline();
        friend class Pipeline;
        friend class Device;
        friend class Swapchain;
        friend class Window;
    };

    class Pipeline
    {
    private:
        /* owner */
        RenderPass* p_render_pass;
        /* self */
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;
    public:
        friend class Swapchain;
        friend class Device;
        friend class RenderPass;
    };

    class Swapchain
    {
    private:
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;
        std::vector<VkImageView> swapchain_image_views;
        std::vector<VkFramebuffer> swapchain_framebuffers;
    public:
        void submit(Device* device, Pipeline* pipeline, CommandBuffer* command_buffer, Sync* sync);
        friend class Pipeline;
        friend class CommandBuffer;
        friend class Instance;
        friend class Window;
    };

    class CommandPool
    {
    private:
        /* owner */
        Device* p_device;
        /* self */
        VkCommandPool command_pool;
        std::vector<CommandBuffer*> command_buffers;
    public:
        CommandBuffer* createBuffer();
        friend class Swapchain;
        friend class Device;
    };

    class CommandBuffer
    {
    private:
        CommandPool* p_pool;
        VkCommandBuffer commandBuffer;
    public:
        friend class CommandPool;
        friend class Swapchain;
    };

    class Sync
    {
    private:
        /* owner */
        Device* p_device;
        /* self */
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;
    public:
        void wait(Device* device);
        friend class Swapchain;
        friend class Device;
    };

    
    
}

namespace PWEngine::Render 
{
    
}
