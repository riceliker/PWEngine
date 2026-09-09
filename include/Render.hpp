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

#define MAX_FRAMES_IN_FLIGHT 3

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
    class Texture;
    class Pipeline;
    class CommandBuffer;
    class PipelineCommand;
    class SimpleTimeCommand;
    class Sync;
    class FrameSubmitCommand;
    class Mesh2D;
    class Texture2D;
    class UBO;

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
        void createDescriptorPool();
    public:
        Stream::LogSystem* log;
        Instance* p_instance;
        uint32_t current_frame = 0;
        struct Impl;
        std::unique_ptr<Impl> self;
        /* Constructor */
        RenderContext();
        ~RenderContext();
        /* Preload */
        size_t addDescriptorSetLayout();
        size_t addDescriptorSet(size_t descriptor_set_layout_index);
        size_t addRenderPass();
        void createSwapchain(size_t render_pass_index);
        void recreateSwapchain();
        std::unique_ptr<Pipeline> createPipeline(size_t render_pass_index, size_t descriptor_set_layout_index);
        /* Buffer */
        std::unique_ptr<Mesh2D> createMesh2D(std::vector<Utils::Vertex2D> vertices, std::vector<uint32_t> indices);
        std::unique_ptr<UBO> createUBO();
        std::unique_ptr<Texture2D> createTexture2D(Utils::ImageRGBA8* const image);
        /* Loop */
        void updateDescriptor(size_t descriptor_set_index, UBO* ubo, Texture2D* texture);
        void drawFrameCommand(Pipeline* pipeline, size_t descriptor_set_index, std::function<void(FrameSubmitCommand& cmd)> func);
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
    public:
        struct Impl;
        std::unique_ptr<Impl> self;
        Pipeline();
        ~Pipeline();
        friend class RenderContext;
    };

}

namespace PWEngine::Render
{

    class Mesh2D
    {
    private:
        void setVertices();
        void setIndices();
    public:
        RenderContext* p_context;
        std::vector<Utils::Vertex2D> vertices;
        std::vector<uint32_t> indices;
        struct Impl;
        std::unique_ptr<Impl> self;
        Mesh2D();
        ~Mesh2D();
        friend class RenderContext;
    };

    class UBO
    {
    private:
    public:
        struct UBOData
        {
            Utils::Mat4 model{0};
            Utils::Mat4 view{0};
            Utils::Mat4 project{0};
        };
        UBOData data;
        RenderContext* p_context;
        struct Impl;
        std::unique_ptr<Impl> self;
        UBO();
        ~UBO();
        friend class RenderContext;
    };

    class Texture2D
    {
    private:
        void createTexture(RenderContext* context, Utils::ImageRGBA8* const image);
        void createView();
        void createSampler();
    public:
        RenderContext* p_context;
        Utils::Vec2<uint32_t> size; 
        struct Impl;
        std::unique_ptr<Impl> self;
        Texture2D();
        ~Texture2D();
        friend class RenderContext;
    };
}

namespace PWEngine::Render
{
    class FrameSubmitCommand
    {
    public:
        RenderContext* p_context;
        struct Impl;
        std::unique_ptr<Impl> self;
        FrameSubmitCommand();
        void setViewPort();
        void setScissor();
        void addMesh2D(Mesh2D* mesh);
        void updateUBO(UBO* ubo, float time);
    };  
    
}


