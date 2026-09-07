#pragma once
#include "stream.hpp"
#include <cstddef>
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

#define MAX_FRAMES_IN_FLIGHT 8

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
friend class VertexBuffer; \
friend class PipelineCommand; \
friend class SimpleTimeCommand;


namespace PWEngine::Render 
{
    struct Vertex
    {    
        Utils::Vec2<float> position;
        Utils::Vec3<float> color;
        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
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
    class RenderContext;
    class Device;
    class Window;
    class VertexBuffer;
    class Texture;
    class RenderPass;
    class Pipeline;
    class Swapchain;
    class CommandPool;
    class CommandBuffer;
    class PipelineCommand;
    class SimpleTimeCommand;
    class Sync;
    class FrameSubmitCommand;

    /*
        The class control the vulkan instance.
        Instance -> Device;
    */
    class Instance
    {
    private:
        std::vector<RenderContext*> context_list;
        bool checkValidationLayer();
        void createInstance(InstanceInfo info);
        void getBestAdapter();
    public:
        /* Log */
        Stream::LogSystem* log;
        /* PImpl */
        struct Impl;
        std::unique_ptr<Impl> self;
        /* Constructor */
        Instance(InstanceInfo info, Stream::LogSystem* log);
        ~Instance();
        /* User Function*/
        RenderContext* createContext(WindowInfo info);
    };

    class RenderContext
    {
    private:
        size_t index;
        void createDevice();
        void createWindow(WindowInfo info);
        void createSync();
        void createCommandPool();
    public:
        Stream::LogSystem* log;
        Instance* p_instance;
        uint32_t current_frame = 0;
        struct Impl;
        std::unique_ptr<Impl> self;
        /* Constructor */
        RenderContext();
        ~RenderContext();
        /* Function */
        size_t addRenderPass();
        void createSwapchain(size_t render_pass_index);
        void recreateSwapchain();
        std::unique_ptr<Pipeline> createPipeline(size_t render_pass_index);
        /* Buffer */
        std::unique_ptr<VertexBuffer> createVertexBuffer(std::vector<Vertex> vertices);

        /* Command */
        void drawFrame(Pipeline* pipeline, std::function<void(FrameSubmitCommand& cmd)> func);
        /* check the window is closed? */
        bool getIsClosed();
        /* when leave the main loop, call it. */
        void waitIdle();
        friend class Instance;
    };

    class Pipeline
    {
    private:
        /* owner */
        RenderContext* p_context;
        VkRenderPass p_render_pass;
        /* self */
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;
    public:
        ~Pipeline();
        friend class RenderContext;
    };

}

namespace PWEngine::Render
{

    class VertexBuffer
    {
    private:
    public:
        RenderContext* p_context;
        std::vector<Vertex> vertices;
        struct Impl;
        std::unique_ptr<Impl> self;
        VertexBuffer();
        ~VertexBuffer();
    };

    struct Texture
    {
        Device* p_device;
        struct Impl;
        std::unique_ptr<Impl> self;
        Utils::Vec2<size_t> size; 
        ~Texture();
    };
}

namespace PWEngine::Render
{
    

    class CommandPool
    {
    private:
        /* owner */
        Device* p_device;
        /* self */
        VkCommandPool ptr;
    public:
        ~CommandPool();
        PipelineCommand createPipelineCommand();
        void createSimpleTimeCommand(std::function<void(SimpleTimeCommand cmd)> func);
        void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, Texture texture);
        __PWEngine_Render_Friend_Class_Define()
    };

    class FrameSubmitCommand
    {
    public:
        struct Impl;
        std::unique_ptr<Impl> self;
        FrameSubmitCommand();
        void setViewPort();
        void setScissor();
        void addVertexBuffer(VertexBuffer* vertex_buffer);
    };  


    template<typename T>
    class UniformBuffer
    {
        T object;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;
    };
}


