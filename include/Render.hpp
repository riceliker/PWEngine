/*
                PWEngine Render Module      

    This module will use Vulkan API to help you build the 
    basic render system.
    The Render Module is a explicit API, not status machine.
    Don't worry. It's very easy.
    The first thing is RenderInstance. All RenderContext will
    be create by it. RenderContext bind a Window, Device and
    Swapchain. You just registry the RenderPass and RenderPipeline.

 */

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

namespace PWEngine::Render
{
    struct InstanceInfo
    {
        std::string name;
        Utils::Vec3<uint8_t> version;
        bool is_debug;
    };

    struct ContextInfo
    {
        Utils::Vec2<uint32_t> window_default_resolution;
        std::string window_title;
        bool is_window_resizable;
    };

    struct CameraInfo
    {
        Utils::Vec3<float> camera_pos;
        Utils::Vec3<float> camera_look_pos;
    };

    class RenderInstance;
    class RenderContext;
    class Texture;
    class Pipeline;
    class CommandBuffer;
    class PipelineCommand;
    class SimpleTimeCommand;
    class Sync;
    class FrameSubmitCommand;
    class Mesh3D;
    class Texture2D;
    class UBO;

    /*
        The class control the vulkan instance.
        RenderInstance -> Device;
    */
    class RenderInstance
    {
    private:
        bool is_debug;
        std::string application_name;
        Utils::Vec3<uint32_t> application_version;
        std::vector<RenderContext> contexts;
    public:
        /* Log */
        Stream::LogSystem* log;
        /* PImpl */
        struct Impl;
        std::unique_ptr<Impl> self;
        /* Constructor */
        RenderInstance(Stream::LogSystem* log);
        void openValidationLayer();
        void setApplicationName(std::string name);
        void setApplicationVersion(Utils::Vec3<uint32_t> version);
        void build();
        ~RenderInstance();
        /* User Function*/
        std::shared_ptr<RenderContext> createContext(ContextInfo info);
    };

    class RenderContext
    {
    private:
        size_t index;
        /* init */
        void createSync();
        void createCommandPool();
        void createDescriptorPool();
    public:
        Stream::LogSystem* log;
        RenderInstance* p_instance;
        uint32_t current_frame = 0;
        struct Device;
        std::unique_ptr<Device> m_device;
        struct Window;
        std::unique_ptr<Window> m_window;
        struct Swapchain;
        std::unique_ptr<Swapchain> m_swapchain;
        struct Impl;
        std::unique_ptr<Impl> self;
        /* Constructor */
        RenderContext();
        ~RenderContext();
        /* Preload */
        size_t addDescriptorSetLayout();
        size_t addDescriptorSet(size_t descriptor_set_layout_index);
        void recreateSwapchain();
        std::unique_ptr<Pipeline> createPipeline(size_t descriptor_set_layout_index);
        /* Buffer */
        std::unique_ptr<Mesh3D> createMesh3D(std::vector<Utils::Vertex3D> vertices, std::vector<uint32_t> indices);
        std::unique_ptr<UBO> createUBO();
        std::unique_ptr<Texture2D> createTexture2D(Utils::ImageRGBA8* const image);
        /* Loop */
        void updateDescriptor(size_t descriptor_set_index, UBO* ubo, Texture2D* texture);
        void drawFrameCommand(Pipeline* pipeline, size_t descriptor_set_index, std::function<void(FrameSubmitCommand& cmd)> func);
        /* check the window is closed? */
        bool getIsClosed();
        /* when leave the main loop, call it. */
        void waitIdle();
        friend class RenderInstance;
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

    class Mesh3D
    {
    private:
        void setVertices();
        void setIndices();
    public:
        RenderContext* p_context;
        std::vector<Utils::Vertex3D> vertices;
        std::vector<uint32_t> indices;
        struct Impl;
        std::unique_ptr<Impl> self;
        Mesh3D();
        ~Mesh3D();
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
        void addMesh3D(Mesh3D* mesh);
        void updateUBO(UBO* ubo, float time, CameraInfo camera);
    };  
    
}


