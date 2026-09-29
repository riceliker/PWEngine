// This file is part of PWEngine.
// PWEngine is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PWEngine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with PWEngine.  If not, see <https://www.gnu.org/licenses/>.
// Copyright (C) 2026 riceliker & all contributors
#pragma once
#include "stream.hpp"
#include "math.hpp"
#include "api.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/* The number of frame in swapchain. The 3 is best now. */
#define MAX_FRAMES_IN_FLIGHT 3

namespace PWEngine::Render
{
/*    Structure Of PWEngine Render Module   */
class RenderInstance;
/*    |                                                                                                 */ 
/*    |--> */class RenderContext;/* ------------------------------------------------------------------->*/         
/*    |            |                                                                                    */
/*    |            V                                                                                    */
/*    |    */class Pipeline3D;/*------------------------------------------------------->*/class Input;
/*    |            |                |                    |                 |                  |         */
/*    |            V                V                    V                 V                  |         */
/*    |    */class Mesh3D;   class Uniform;      class Texture2D;    class Camera3D;/*        |         */
/*    |            |                |                    |                 |                  |         */
/*    |            |                V                    V                 |                  |         */
/*    |            |<-----*/class Transform3D;    class Material;/*        |                  |         */
/*    |            |                                     |                 |                  |         */
/*    V            V                                     V                 V                  |         */
class Command;/*<---------------------------------------------------------------------------------------*/

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
        Vec2<uint32_t> window_default_resolution;
        std::string window_title;
        bool is_window_resizable;
    };
    /*
        The data type of DescriptorSetLayout.
        - Uniform: The uniform buffer
        - Sampler: The image buffer
        - Storage: The multi-uniform buffer
    */
    enum class DescriptorType
    {
        Uniform, Sampler, Storage
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
    /*
        The RenderInstance is the start of PWEngine::Render.
        And the information about application will be registry here.
    */
    class RenderInstance
    {
    private:
        bool is_debug;
        std::string application_name;
        Vec3<uint32_t> application_version;
        std::vector<RenderContext> contexts;
    public:
        Stream::LogSystem* log;

        struct Impl;
        std::unique_ptr<Impl> self;

        RenderInstance(Stream::LogSystem* log);
        void openValidationLayer();
        void setApplicationName(std::string name);
        void setApplicationVersion(Vec3<uint32_t> version);
        void build();
        ~RenderInstance();

        std::shared_ptr<RenderContext> createContext(ContextInfo info);
    };

/*
     ███████     ███████░   ███░    ██ ▓█████████ █████████ ▓██▒   ███ █████████
    ███    ███  ███    ███  █████   ██     ██▒    ███         ███ ██▒     ███
   ███         ███      ██▒ ██████  ██     ██▒    ████████▒    ████       ███
   ███         ███      ██▒ ███ ▓██ ██     ██▒    ███          ████▒      ███
    ██▓    ███  ███    ███  ███   ████     ██▒    ███        ▒██░ ███     ███
     ███████     ███████▓   ███    ███     ██▒    █████████ ███    ███    ███
*/

    class RenderContext
    {
    private:
        /* call vkQueueSumbit to submit the command buffer. */
        void frameSubmit(size_t swapchain_loop_frame_index, uint32_t* swapchain_image_index);
        /* get GLFW event.*/
        void pollEvents();
        /* check window is closed by GLFW. */
        bool getIsWindowClosed();
        void waitIdle();
        /* wait fence to wait GPU finished this task and send CPU data. */
        void waitFence(size_t swapchain_loop_frame_index, uint32_t* swapchain_image_index);
        Command* commandBegin(size_t swapchain_loop_frame_index, uint32_t swapchain_image_index);
        void commandEnd(Command* command);
        std::shared_ptr<Input> createInput();
    public:
        RenderInstance* p_instance;
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
        std::shared_ptr<Texture2D> createTexture2D(ImageRGBA8* surface);
        std::shared_ptr<Transform3D> createTransform3D();
        std::shared_ptr<Uniform> createUniform(size_t size, DescriptorType type);
        void hideMouseCursor();
        void showMouseCursor();
        /* Loop */
        template<std::invocable<Command*, Input*, float> F> void frameLoop(F&& func);      
        friend class RenderInstance;
    };

    template<std::invocable<Command*, Input*, float> F> void RenderContext::frameLoop(F&& func)
    {
        float delta = 0;
        size_t swapchain_loop_frame_index = 0;
        uint32_t swapchain_image_index = 0;
        auto input = this->createInput();
        while (!this->getIsWindowClosed()) 
        {
            auto start_time = std::chrono::high_resolution_clock::now();
            this->waitFence(swapchain_loop_frame_index, &swapchain_image_index);
            auto cmd = this->commandBegin(swapchain_loop_frame_index, swapchain_image_index);
            this->pollEvents();
            func(cmd, input.get(), delta);
            this->commandEnd(cmd);
            this->frameSubmit(swapchain_loop_frame_index, &swapchain_image_index);
            swapchain_loop_frame_index = (swapchain_loop_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
            auto current_time = std::chrono::high_resolution_clock::now();
            delta = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
        }
        this->waitIdle();
    } 
/*
    ████████   ███  ████████   █████████  ███       ███  ███     ██  ░█████████
    ██    ▓██  ███  ███    ██▒ ███        ███       ███  █████   ██  ░██
    ██    ███  ███  ███    ██░ █████████  ███       ███  ██████  ██  ░████████
    ████████   ███  ████████   ███        ███       ███  ███ ▓██ ██  ░██
    ██         ███  ███        ███        ███       ███  ███   ████  ░██
    ██         ███  ███        █████████  █████████ ███  ███    ███  ░█████████
*/       

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
        std::unique_ptr<Mesh3D> createMesh3D(Model3D* model);
        std::unique_ptr<Material> createMaterial();
        std::unique_ptr<Camera3D> createCamera();
        friend class RenderContext;
    };

    class Uniform
    {
    private:
        void* data;
    public:
        RenderContext* p_context;
        struct Impl;
        std::unique_ptr<Impl> self;
        void updateData(Command* command, void* data, size_t size);
        Uniform(RenderContext* p_context);
        ~Uniform();
    };

    struct Mesh3DNodeTransform
    {
        Mat4 transform = Mat4(1);
    };

    class Texture2D
    {
    private:
        void setTexture2D(ImageRGBA8* surface);
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

/*
   ▒███    ██    ███████    ████████    █████████
   ▒████   ██   ███    ███  ███    ███  ███
   ▒██▒██  ██  ███      ██░ ███     ██  ████████
   ▒██  ██ ██  ███      ██░ ███     ██  ███
   ▒██   ████   ██▓    ███  ███    ███  ███
   ▒██    ███    ███████░   ████████░   █████████
*/ 

    class Transform3D
    {
    private:
        std::vector<Vec3<float>> positions;
        std::vector<Vec4<float>> rotations;
        std::vector<Vec3<float>> scales;
    public:
        size_t length;
        std::shared_ptr<Uniform> m_uniform;
        void resizeTransform(size_t size);
        void addTransform();
        Vec3<float> getPosition(size_t index);
        void setPosition(size_t index, Vec3<float> new_position);
        void addPosition(size_t index, Vec3<float> delta_postion);
        Vec3<float> getRotation(size_t index);
        void setRotation(size_t index, Vec3<float> new_rotation);
        void addRotation(size_t index, Vec3<float> delta_rotation);
        Vec3<float> getScale(size_t index);
        void setScale(size_t index, Vec3<float> new_scale);
        void addScale(size_t index, Vec3<float> delta_scale);
        void calculateNodeMatrix(Command* command);
        Transform3D();
    };

    class Mesh3D
    {
    private:
        void setVertices(RenderContext* super, std::vector<Vertex3D>& vertices);
        void setIndices(RenderContext* super, std::vector<uint32_t>& indices);
    public:
        /* parent */
        Pipeline3D* p_pipeline;
        struct DescriptorSet;
        std::unique_ptr<DescriptorSet> m_descriptor_set;
        std::shared_ptr<Transform3D> transform;
        size_t indices_size;
        /* impl */
        struct Vertex3D;
        std::unique_ptr<Vertex3D> m_vertex;
        /* public */
        void bindTransform3D(std::shared_ptr<Transform3D> transform);
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
        void updateDescriptor();
        Material();
        ~Material();
        friend class Pipeline3D;
    };

    class Node3D
    {
    private:
        std::optional<std::shared_ptr<Mesh3D>> m_mesh;
        std::optional<std::shared_ptr<Material>> m_material;
        std::optional<std::shared_ptr<Transform3D>> m_transform;
    public:
        Pipeline3D* p_pipeline;
        struct DescriptorSet;
        std::unique_ptr<DescriptorSet> m_descriptor_set; 
        Node3D(Pipeline3D* pipeline);
        ~Node3D();
        void bindMesh3D(std::shared_ptr<Mesh3D> mesh3D);
        void bindMaterial(std::shared_ptr<Material> material);
        void bindTransform3D(std::shared_ptr<Transform3D> transform3D);
        void updateDescriptor();
    };

    class Camera3D
    {
    public:
        struct CameraData
        {
            Mat4 view{0};
            Mat4 project{0};
        };
        RenderContext* p_context;
        Pipeline3D* p_pipeline;
        Vec3<float> position;
        Vec3<float> look_vector;
        float view_degree = 45;
        Vec2<float> view_depth = {0.01, 100};
        CameraData data;
        struct DescriptorSet;
        std::unique_ptr<DescriptorSet> m_descriptor_set;
        struct Uniform;
        std::unique_ptr<Uniform> m_uniform;
        Camera3D();
        ~Camera3D();
        void calculateLookVector(Vec2<float> look_degree);
        void cameraMoveFromLook(Vec2<float> step);
        void updateDescriptor();
        void updateData(Command* command);
        friend class Pipeline3D;
    };
/*
     ███████     ███████    ████     ████  ████    ████     ████    ░███    ██  ▒████████
    ███    ██▒  ███    ███  █████   █████  ████░   ████    ░████    ░████   ██  ▒██    ███
   ███         ███      ██  ██▓██  ▓█████  ██ ██  ██░██    ██ ░██   ░██▒██  ██  ▒██     ███
   ███         ███      ██  ██▓ ██ ██ ███  ██ ███▒██ ██   ███  ███  ░██  ██ ██  ▒██     ███
   ░██▒    ██▒  ██▓    ███  ██▓ ████  ███  ██  ████  ██  █████████▓ ░██   ████  ▒██    ███
     ███████     ███████    ██▓  ███  ███  ██   ██   ██ ░██     ░██ ░██    ███  ▒████████
*/

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
        uint32_t swapchain_image_index = 0; /* the DS image index */
        void setViewPort();
        void setScissor();
        void setPipeline(Pipeline3D* pipeline);
        void renderingBegin(Vec4<float> color);
        void renderingEnd();
        void draw(Pipeline3D* pipeline, Mesh3D* mesh, Material* material, Camera3D* camera);
        friend class RenderContext;
    };

/*
    ██░  ███    ▓██  █████████  ░██     ███ ██████████
    ██░  ████   ▓██  ███    ███ ░██     ███     ██▓
    ██░  ██ ██▓ ▓██  ███    ███ ░██     ███     ██▓
    ██░  ██  ███▓██  ████████▓  ░██     ███     ██▓
    ██░  ██   █████  ███         ██▓    ███     ██▓
    ██░  ██     ███  ███          ████████      ██▓
*/

    class Input
    {
    private:
        RenderContext* p_context;
    public:
        Input(){};
        ~Input(){};
        bool checkIsKeyInput(Key key);
        bool checkIsMouseInput(MouseKey key);
        Vec2<float> checkMousePosition();
        bool checkIsHoverScreen();
        friend class RenderContext;
    };  
}


