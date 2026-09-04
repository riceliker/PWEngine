#pragma once
#include "stream.hpp"
#include <cstdint>
#include <memory>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif

#include <string>
#include <vector>
#include <functional>
#include <memory>

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
friend class Sync; \
friend class VertexBuffer;


namespace PWEngine::Render 
{
    class Vertex
    {
    private:    
        Utils::Vec2<float> position;
        Utils::Vec3<float> color;
        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    public:
        Vertex(Utils::Vec2<float> position, Utils::Vec3<float> color):position(position),color(color){};
        friend class RenderPass;
    };

    
}


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
    class VertexBuffer;
    class RenderPass;
    class Pipeline;
    class Swapchain;
    class CommandPool;
    class CommandBuffer;
    class Sync;

    class Instance
    {
    public:
        struct Impl;
        std::unique_ptr<Impl> self;
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
    public:
        struct Impl;
        std::unique_ptr<Impl> self;
        Device();
        ~Device();
        Window* createWindow(WindowInfo info);
        RenderPass* createRenderPass();
        CommandPool* createCommandPool();
        Sync* createSync();
        VertexBuffer* createVertexBuffer(std::vector<Vertex> vertices);
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

    class VertexBuffer
    {
    private:
        /* owner */
        Device* p_device;
        std::vector<Vertex> vertices;
        /* self*/
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        ~VertexBuffer();
    public:
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
        Pipeline* createPipeline(Vertex vertex);
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
        void submit(RenderPass* render_pass, CommandBuffer* command_buffer, Sync* sync, std::function<void(CommandBuffer* cmd)> func);
        __PWEngine_Render_Friend_Class_Define()
    };

    class CommandPool
    {
    private:
        /* owner */
        Device* p_device;
        /* self */
        VkCommandPool ptr;
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
        VkCommandBuffer ptr;
    public:
        void bindPipeline(Pipeline* pipeline);
        void setViewPort(Swapchain* swapchain);
        void setScissor(Swapchain* swapchain);
        void addVertexBuffer(VertexBuffer* vertex_buffer);
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


