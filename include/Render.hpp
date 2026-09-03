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

#define __PWEngine_Render_Friend_Class_Define() \
friend class Instance; \
friend class Device; \
friend class Window; \
friend class RenderPass; \
friend class Pipeline; \
friend class Swapchain; \
friend class CommandPool; \
friend class CommandBuffer; \
friend class Sync;

/*
----- GPU Module -----
The GPU module is control the vulkan instance with C++ class.
Use it is simple, you should not write the template code.

Registry:
Instance -> N * Device
Device -> N * Window
Device -> N * RenderPass
Window + RenderPass -> Swapchain
RenderPass -> N * Pipeline
Device -> N * CommandPool
CommandPool -> N * CommandBuffer
Device -> N * Sync
Loop:
Swapchain <- 

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
        /* self */
        bool is_debug = false;
        VkInstance ptr;
        VkDebugUtilsMessengerEXT debug_messenger;
        std::vector<VkPhysicalDevice> adapters;
        std::vector<Device*> devices;
        /* func */
        void createInstance(InstanceInfo info);
        void getAllAdapter();
    public:
        Instance(InstanceInfo info, Stream::LogSystem* log);
        ~Instance();
        Device* GetBestDevice();
        __PWEngine_Render_Friend_Class_Define()
    };
    /*
        Owner: Instance
        Manager: Window RenderPass CommandPool Sync
    */
    class Device
    {
    private:
        /* owner */
        Instance* p_instance;
        /* manger */
        std::vector<Window*> windows;
        std::vector<RenderPass*> render_passes;
        std::vector<CommandPool*> command_pools;
        std::vector<Sync*> syncs;
        /* self */
        VkPhysicalDevice adapter;
        VkDevice ptr;
        VkQueue graphics_queue;
        void registryWindow();
    public:
        ~Device();
        Window* createWindow(WindowInfo info);
        RenderPass* createRenderPass();
        CommandPool* createCommandPool();
        Sync* createSync();
        void waitIdle();
        __PWEngine_Render_Friend_Class_Define()
    };
    /*
        Owner: Device;
        Mananger: NULL
    
    */
    class Window
    {
    private:
        /* parent */
        Device* p_device;
        /* self */
        GLFWwindow* ptr;
        VkSurfaceKHR surface;
        std::vector<Swapchain*> swapchains;
    public:
        ~Window();
        Swapchain* createSwapchain(RenderPass* render_pass);
        bool isClosed();
        __PWEngine_Render_Friend_Class_Define()
    };
    /*
        Owner: Device
        Manager: Pipeline
    */
    class RenderPass
    {
    private:
        /* owner */
        Device* p_device;
        /* self*/
        VkRenderPass ptr;
        std::vector<Pipeline*> pipelines;
    public:
        ~RenderPass();
        Pipeline* createPipeline();
        __PWEngine_Render_Friend_Class_Define()
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
        ~Pipeline();
        __PWEngine_Render_Friend_Class_Define()
    };

    class Swapchain
    {
    private:
        /* owner */
        Window* p_window;
        /* self */
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchain_images;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;
        std::vector<VkImageView> swapchain_image_views;
        std::vector<VkFramebuffer> swapchain_framebuffers;
    public:
        ~Swapchain();
        void submit(Pipeline* pipeline, CommandBuffer* command_buffer, Sync* sync);
        __PWEngine_Render_Friend_Class_Define()
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
        ~CommandPool();
        CommandBuffer* createBuffer();
        __PWEngine_Render_Friend_Class_Define()
    };

    class CommandBuffer
    {
    private:
        CommandPool* p_pool;
        VkCommandBuffer commandBuffer;
    public:
        __PWEngine_Render_Friend_Class_Define()
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
        ~Sync();
        void wait(Device* device);
        __PWEngine_Render_Friend_Class_Define()
    };

    
    
}

namespace PWEngine::Render 
{
    
}
