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

#define MAX_FRAMES_IN_FLIGHT 3

namespace PWEngine::Render
{
/*    Structure Of PWEngine Render Module   */
 class RenderInstance;
    /*    |    */
    /*    |    */ 
    /*    |--> */class RenderContext;/* ------------------------------------> */         
    /*    |            |                               |                 |    */
    /*    |            V                               V                 V    */
    /*    |    */class Pipeline3D;/*->*/        class Texture2D;   class Input;
    /*    |            |             |                 |                 |    */
    /*    |            V             V                 V                 |    */
    /*    |    */class Mesh3D;   class Camera;  class Material;    /*    |    */
    /*    |            |             |                 |                 |    */
    /*    V            V             V                 V    */     /*    V    */
    class Command;/*<------------------------------------------------------------*/
 
    
/*
     ███████     ███████    ███    ██░  ████████ ███   ▓██████         ███████  █████████ ░████████   ███    ███   ░██████▒  █████████
    ██▓   ░██  ▒██░   ░██░  ████   ██░  ██       ███  ███    ███      ███   ███    ███    ░██    ███  ███    ███  ███    ███    ███
   ███         ██▒     ▓██  ██ ██  ██░  ██▒▒▒▒▒  ███ ███              ▓█████       ███    ░██    ███  ███    ███ ░██            ███
   ███         ██▒     ▓██  ██  ██▒██░  ███████  ███ ███   █████          █████    ███    ░████████   ███    ███ ░██            ███
   ░██░   ░██  ▓██     ██▓  ██   ████░  ██       ███  ███    ███      ██░    ██    ███    ░██    ▓██  ███    ███  ███    ███    ███
     ███████     ███████    ██    ███░  ██       ███   █████████       ███████     ███    ░██     ██   ████████    ▒██████▓     ███
*/

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

    enum class Key : uint16_t
    {
        Unknown = 0,    
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,  
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        Space, Enter, Tab, Backspace,
        Left, Right, Up, Down,
        Insert, Delete, Home, End, PageUp, PageDown,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Kp0, Kp1, Kp2, Kp3, Kp4, Kp5, Kp6, Kp7, Kp8, Kp9,
        KpDecimal, KpDivide, KpMultiply, KpSubtract, KpAdd, KpEnter,
        LShift, RShift, LCtrl,  RCtrl, LAlt, RAlt, LSuper, RSuper,
        Menu, Escape
    };

    enum MouseKey : uint8_t
    {
        Unknown = 0,
        BtnLeft, BtnRight, BtnMiddle
    };
/*
    ██  ░███    ███   ███████  ██████████  ████     ███    ░██    ███████    █████████
    ██  ░████   ███  ██    ███    ▓██      ████▓    ████░  ░██  ▓██▒    ██▓  ██
    ██  ░██ ██  ███  █████▓       ▓██     ██  ██    ██░███ ░██  ███          ████████
    ██  ░██  ██▓███     ░█████    ▓██    ███  ▓██   ██░ ███░██  ██▓          ██
    ██  ░██   █████ ███     ██    ▓██   ██████████  ██░  ▒████  ███     ██▓  ██
    ██  ░██    ▓███  ▓███████     ▓██  ░██      ███ ██░    ███    ███████    █████████
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
        void frameSubmit(size_t swapchain_loop_frame_index);
        void pollEvents();
        bool getIsWindowClosed();
        void waitIdle();
        void waitFence(size_t swapchain_loop_frame_index);
        Command* commandBegin(size_t swapchain_loop_frame_index);
        void commandEnd(Command* command);
        std::shared_ptr<Input> createInput();
    public:
        /* input */
        bool checkIsInput(int key);
        void checkMouse(bool& flag);
        Utils::Vec2<int> getMouse();
        void hideMouseCursor();
        void showMouseCursor();
        /* log system */
        Stream::LogSystem* log;
        RenderInstance* p_instance;
        /* loop variable */
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
        template<std::invocable<Command*, Input*, float> F> void frameLoop(F&& func);      
        friend class RenderInstance;
    };

    template<std::invocable<Command*, Input*, float> F> void RenderContext::frameLoop(F&& func)
    {
        float delta = 0;
        size_t swapchain_loop_frame_index = 0;
        auto input = this->createInput();
        while (!this->getIsWindowClosed()) 
        {
            auto start_time = std::chrono::high_resolution_clock::now();
            this->waitFence(swapchain_loop_frame_index);
            auto cmd = this->commandBegin(swapchain_loop_frame_index);
            this->pollEvents();
            func(cmd, input.get(), delta);
            this->commandEnd(cmd);
            this->frameSubmit(swapchain_loop_frame_index);
            swapchain_loop_frame_index = (swapchain_loop_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
            auto current_time = std::chrono::high_resolution_clock::now();
            delta = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
        }
        this->waitIdle();
    }        

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
        std::unique_ptr<Camera> createCamera();
        friend class RenderContext;
    };

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
        void update(uint32_t swapchain_loop_frame_index);
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
        void update(size_t swapchain_loop_frame_index);
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
        void update(uint32_t swapchain_loop_frame_index);
        friend class Pipeline3D;
    };

    class Command
    {
    private:
        struct Impl;
        std::unique_ptr<Impl> self;
        RenderContext* p_context;
    public:
        Command(); 
        ~Command()=default;
        size_t swapchain_loop_frame_index;
        void setViewPort();
        void setScissor();
        void setPipeline(Pipeline3D* pipeline);
        void renderingBegin(Utils::Vec4<float> color);
        void renderingEnd();
        void draw(size_t current_frame, Pipeline3D* pipeline, Mesh3D* mesh, Material* material, Camera* camera);
        friend class RenderContext;
    };

    class Input
    {
    private:
        RenderContext* p_context;
    public:
        Input(){};
        ~Input(){};
        bool checkIsKeyInput(Key key);
        bool checkIsMouseInput(MouseKey key);
        Utils::Vec2<float> checkMousePosition();
        bool checkIsHoverScreen();
        friend class RenderContext;
    };  
}


