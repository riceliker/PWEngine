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
#include "utils.hpp"
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <memory>

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
    class Texture2D;
    class Material;
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
        void pollEvents();
        bool getIsWindowClosed();
        void waitIdle();
        FrameCommandFactory* __frameCommandStart();
        void __frameCommandEnd(FrameCommandFactory* factory);
        void waitFence();
        /* log system */
        Stream::LogSystem* log;
        RenderInstance* p_instance;
        /* loop variable */
        uint32_t current_frame = 0; /* the swapchain frame */
        uint32_t image_index = 0; /* the DS image index */
        /* PImpl */
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
        /* create */
        void recreateSwapchain();
        std::unique_ptr<Pipeline3D> createPipeline3D(std::vector<std::vector<ShaderLayoutInfo>> infos, ShaderPath path);
        std::shared_ptr<Texture2D> createTexture2D(Utils::ImageRGBA8* surface);
        /* Loop */
        template<std::invocable<float> F> void frameLoop(F&& func)
        {
            float delta = 0;
            while (!this->getIsWindowClosed()) 
            {
                auto start_time = std::chrono::high_resolution_clock::now();
                this->pollEvents();
                this->waitFence();

                func(delta);

                this->current_frame = (this->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
                
                auto current_time = std::chrono::high_resolution_clock::now();
                delta = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
            }
            this->waitIdle();
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
        std::unique_ptr<Mesh3D> createMesh3D(Utils::Model3D* model);
        std::unique_ptr<Material> createMaterial();
        std::unique_ptr<Camera> creatCamera();
        friend class RenderContext;
    };

}

namespace PWEngine::Render
{
    class Texture2D
    {
    private:
        void setTexture2D(Utils::ImageRGBA8* surface);
        void setTextureView();
        void setTextureSampler();
    public:
        RenderContext* p_context;
        struct Impl;
        std::unique_ptr<Impl> self;
        Texture2D();
        ~Texture2D();
        void setBasicTexture();        
        friend class RenderContext;
    };

    class Node3D
    {  
    protected:
        Utils::Vec3<float> position;
        Utils::Vec4<float> rotation;
        Utils::Vec3<float> scale;
        virtual void calculateNodeMatrix() = 0;
    public:
        Utils::Vec3<float> getPosition();
        void setPosition(Utils::Vec3<float> new_position);
        void addPosition(Utils::Vec3<float> delta_postion);
        Utils::Vec3<float> getRotation();
        void setRotation(Utils::Vec3<float> new_rotation);
        void addRotation(Utils::Vec3<float> delta_rotation);
        Utils::Vec3<float> getScale();
        void setScale(Utils::Vec3<float> new_scale);
        void addScale(Utils::Vec3<float> delta_scale);
    };

    class Mesh3D : public Node3D
    {
    private:
        void setVertices(RenderContext* super, std::vector<Utils::Vertex3D>& vertices);
        void setIndices(RenderContext* super, std::vector<uint32_t>& indices);
        void setUniform(Pipeline3D* super);
    public:
        /* parent */
        Pipeline3D* p_pipeline;
        struct DescriptorSet;
        std::unique_ptr<DescriptorSet> m_descriptor_set;
        /* uniform */
        struct UniformData
        {
            Utils::Mat4 transform = Utils::Mat4(1);
        };
        UniformData data;
        size_t indices_size;
        /* impl */
        struct Vertex3D;
        std::unique_ptr<Vertex3D> m_vertex;
        struct Uniform;
        std::unique_ptr<Uniform> m_uniform;
        /* public */
        void calculateNodeMatrix();
        void update(uint32_t current_frame);
        void draw(FrameCommandRendering* rendering);
        /* constructor */
        Mesh3D();
        ~Mesh3D();
        friend class Pipeline3D;
    };

    /* set 1 */
    class Material
    {
    private:
        void createDescriptorSet();
        std::optional<std::shared_ptr<Texture2D>> basic_texture;
        std::optional<std::shared_ptr<Texture2D>> normal_texture;
        std::optional<std::shared_ptr<Texture2D>> metallic_roughness_texture;
        std::optional<std::shared_ptr<Texture2D>> ao_texture;
        std::optional<std::shared_ptr<Texture2D>> emissive_texture;
    public:

        Pipeline3D* p_pipeline;
        struct DescriptorSet;
        std::unique_ptr<DescriptorSet> m_descriptor_set;
        struct Uniform;
        std::unique_ptr<Uniform> m_uniform;
        void bindBasicTexture(std::shared_ptr<Texture2D> texture);
        void UpdateDescriptorSets(size_t current_frame);
        Material();
        ~Material();
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
        struct DescriptorSet;
        std::unique_ptr<DescriptorSet> m_descriptor_set;
        struct Uniform;
        std::unique_ptr<Uniform> m_uniform;
        Camera();
        ~Camera();
        void calculateLookVector(Utils::Vec2<float> look_degree);
        void cameraMoveFromLook(Utils::Vec2<float> step);
        void update(uint32_t current_frame);
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
        void draw(size_t currect_frame, Mesh3D* mesh, Material* material, Camera* camera);
    }; 
}


