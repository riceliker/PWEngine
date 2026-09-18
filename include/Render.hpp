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
#include <concepts>
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
#include <memory>

#include "utils.hpp"

#define MAX_FRAMES_IN_FLIGHT 3

namespace PWEngine::Render
{
    struct ContextInfo
    {
        Utils::Vec2<uint32_t> window_default_resolution;
        std::string window_title;
        bool is_window_resizable;
    };

    enum class DescriptorType
    {
        Uniform, Sampler
    };

    enum class ShaderType
    {
        Vertex, Fragment
    };

    struct ShaderPath
    {
        std::string vertex_shader;
        std::string fragment_shader;
    };

    struct ShaderLayoutInfo
    {
        uint32_t binding_index;
        uint32_t binding_array_count;
        DescriptorType descriptor_type;
        ShaderType shader_type;
    };

    class RenderInstance;
    class RenderContext;
    class Pipeline3D;
    class Mesh3D;
    class Camera;
    class DescriptorSet;
    class FrameCommandFactory;
    class FrameCommandRendering;

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
        /* init */
        void createSync();
        void createCommandPool();
        void createDescriptorPool();
    public:
        bool checkIsInput(int key);
        void checkMouse(bool& flag);
        Utils::Vec2<int> getMouse();
        bool __getIsWindowClosed();
        void __waitIdle();
        FrameCommandFactory* __frameCommandStart();
        void __frameCommandEnd(FrameCommandFactory* factory);
        void __waitFence();
        Stream::LogSystem* log;
        RenderInstance* p_instance;
        uint32_t current_frame = 0;
        uint32_t image_index = 0;
        float delta = 0;
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
        void recreateSwapchain();
        std::unique_ptr<Pipeline3D> createPipeline3D(std::vector<std::vector<ShaderLayoutInfo>> infos, ShaderPath path);
        /* Loop */
        template<typename F> void frameLoop(F&& func)
        {
            while (!this->__getIsWindowClosed()) 
            {
                auto start_time = std::chrono::high_resolution_clock::now();
                glfwPollEvents();

                func();

                this->current_frame = (this->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
                
                auto current_time = std::chrono::high_resolution_clock::now();
                this->delta = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
            }
            this->__waitIdle();
        }
        
        template<std::invocable<FrameCommandFactory*> F> void frameCommand(F&& func)
        {
            auto factory = this->__frameCommandStart();
            func(factory);
            this->__frameCommandEnd(factory);
        }
        
        void frameSubmit();
        friend class RenderInstance;
    };

    class Pipeline3D
    {
    private:
    public:
        RenderContext* p_context;
        struct Impl;
        std::unique_ptr<Impl> self;
        /* Constructor */
        Pipeline3D();
        ~Pipeline3D();
        std::unique_ptr<Mesh3D> createMesh3D(Utils::Model3D* model, Utils::ImageRGBA8* image);
        std::unique_ptr<Camera> creatCamera();
        friend class RenderContext;
    };

}

namespace PWEngine::Render
{

    class Mesh3D
    {
    private:
        void setVertices(RenderContext* super, std::vector<Utils::Vertex3D>& vertices);
        void setIndices(RenderContext* super, std::vector<uint32_t>& indices);
        void setTexture2D(RenderContext* super, Utils::ImageRGBA8& surface);
        void setTextureView(RenderContext* super);
        void setTextureSampler(RenderContext* super);
        void setUniform(Pipeline3D* super);
    public:
        struct UBOData
        {
            Utils::Mat4 model{0};
        };
        UBOData data;
        size_t indices_size;
        RenderContext* p_context;
        Pipeline3D* p_pipeline;
        struct Vertex3D;
        std::unique_ptr<Vertex3D> m_vertex;
        struct Uniform;
        std::unique_ptr<Uniform> m_uniform;
        struct Texture2D;
        std::unique_ptr<Texture2D> m_texture;
        void update(uint32_t current_frame);
        void setData();
        void bind(FrameCommandRendering* rendering);
        Mesh3D();
        ~Mesh3D();
        friend class Pipeline3D;
    };

    class Camera
    {
    public:
        struct CameraData
        {
            Utils::Mat4 view{0};
            Utils::Mat4 project{0};
        };
        RenderContext* p_context;
        Pipeline3D* p_pipeline;
        Utils::Vec3<float> position;
        Utils::Vec3<float> look_vector;
        float view_degree = 45;
        Utils::Vec2<float> view_depth = {0.01, 100};
        CameraData data;
        struct Impl;
        std::unique_ptr<Impl> self;
        Camera();
        ~Camera();
        void calculateLookVector(Utils::Vec2<float> look_degree);
        void cameraMoveFromLook(Utils::Vec2<float> step);
        void update(uint32_t current_frame);
        void setData(uint32_t current_frame);
        friend class Pipeline3D;
    };
}

namespace PWEngine::Render
{
    class FrameCommandFactory
    {
    private:
        
    public:
        FrameCommandRendering* __frameRenderingStart();
        void __frameRenderingEnd(FrameCommandRendering* cmd);
        RenderContext* p_context;
        FrameCommandFactory(RenderContext* context): p_context(context){};
        template<std::invocable<FrameCommandRendering*> F> void frameRendering(F&& func)
        {
            
            auto cmd = this->__frameRenderingStart();
            func(cmd);
            this->__frameRenderingEnd(cmd);
        }
    };

    class FrameCommandRendering
    {
    public:
        RenderContext* p_context;
        struct Impl;
        std::unique_ptr<Impl> self;
        FrameCommandRendering(RenderContext* context);
        void setViewPort();
        void setScissor();
        void setPipeline(Pipeline3D* pipeline);
        void setDescriptorSet(Pipeline3D* pipeline);
    }; 
}


