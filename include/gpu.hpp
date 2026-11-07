// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
#pragma once
#include "file.hpp"
#include "math.hpp"
#include "utils.hpp"

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <optional>
#include <unordered_map>
#include <vector>

#include <variant>

#define flight_fences_index_counts 3


/* -------------------------------------------------------------
 | ▒███████▓   ▓████████  ████████ ▒██  ███     ██  █████████ |
 | ▒██    ███  ▓██        ██▓      ▒██  ████▓   ██  ███       |
 | ▒██     ███ ▓████████  ██▓▒▒▒▒  ▒██  ██████  ██  ████████  |
 | ▒██     ███ ▓██        ███████  ▒██  ███ ▓██ ██  ███       |
 | ▒██    ███  ▓██        ██▓      ▒██  ███   ████  ███       |
 | ▒████████   ▓████████  ██▓      ▒██  ███    ███  █████████ |
---------------------------------------------------------------*/
namespace PWEngine::GPU 
{
    /* instance */
    class GPUInstance; class Input; 
    /* allocator */
    class GPUVertex; class GPUImage; class GPUBindingBuffer; class GPUImageView;
    /* pipeline */
    struct RenderPipeline; struct ComputePipeline;
    /* binding */
    class BindingLayout; struct Binding;
    /* task */
    class IGPUTask; class GPUTask; class WindowTask; 
    /* window */
    class Window; class RenderingCommand; class RecordCommand;
}

/*----------------------------------------------------------------------
 |  ▓███████▓  ░██████▓   █████████   ████    ████     ███░  █████████ |
 |  ▓██       ███    ███  ███    ███  ████░   ████    ▓████     ███    |
 |  ▓██▒▒▒▒▒ ░██      ███ ███    ███  ██ ██  ██ ██    ██ ███    ███    |
 |  ▓███████ ▒██      ███ █████████   ██ ███▒██ ██   ██░  ███   ███    |
 |  ▓██       ███    ███  ███    ███  ██  ████  ██  █████████   ███    |
 |  ▓██        ▒███████   ███    ▓██  ██   ██░  ██ ▒██     ███  ███    |
-----------------------------------------------------------------------*/
namespace PWEngine::GPU 
{
    struct Format;
    enum class BufferFormat { i32, u32, f32, vec2, vec3, vec4, color };
    enum class ImageFormat { RGBA8_SRGB, RGB8_SRGB, RGBA8_UNORM, RGB8_UNORM, D_F32_S_U8 };
}

/*-------------------------------------------------------------------
 |  ███████  █████████  ████████   ███    ███   ░██████▒  █████████ |
 | ███   ███    ███     ██    ▓██  ███    ███  ███    ███    ███    |
 | ▒█████       ███     ██    ███  ███    ███ ░██            ███    |
 |     ▓████    ███     ████████   ███    ███ ▒██            ███    |
 | ██░    ██    ███     ██    ▓██  ███    ███  ███    ███    ███    |
 |  ███████     ███     ██     ██   ████████    ▒██████▓     ███    |
--------------------------------------------------------------------*/

namespace PWEngine::GPU
{
    struct GPUInstanceInfo{ bool is_debug; std::string application_name; Vec3<uint32_t> application_version; };
    struct TaskResult { bool should_close = false; };
    struct TaskMassager { enum class Command { create_task, destroy_task} cmd; };

    struct WindowInfo { Vec2<uint32_t> window_default_resolution; std::string window_title; bool is_window_resizable; };

    /* pipeline */
    enum class DescriptorType { Uniform, Sampler, Storage };
    enum class ShaderType { Vertex, Fragment, Compute, VertexFragment, VertexCompute, FragmentCompute, VertexFragmentCompute };
    struct RenderShaderPath { std::string vertex_shader; std::string fragment_shader; };
    struct ShaderLayoutInfo { uint32_t binding_index; uint32_t binding_array_count; DescriptorType descriptor_type; ShaderType shader_type; };
    struct PushConstant { uint32_t offset; uint32_t size; ShaderType type;};
    struct VertexBinding { uint32_t binding; uint32_t size; };
    struct VertexInput { uint32_t binding; uint32_t location; uint32_t offset; enum class Type { u8, i32, f32 } type; enum class Length { vec2, vec3, vec4 } len; };
    enum class BlendOp { add, src_minus_dst, dst_minus_src, min, max };
    enum class BlendObject { zero, one, src_color, one_minus_src_color, dst_color, one_minus_dst_color, src_alpha, one_minus_src_alpha, dst_alpha, one_minus_dst_alpha, constant_color, one_minus_constant_color, constant_alpha, one_minus_constant_alpha, };
    struct ColorBlend { BlendOp color_op; BlendObject color_src; BlendObject color_dst; BlendOp alpha_op; BlendObject alpha_src; BlendObject alpha_dst; Vec4<bool> write_mask;};
    struct DepthStencilInfo { };
    struct RenderingInfo { };
    struct RenderPassInfo { };
    struct RenderPipelineInfo {
        std::variant<std::vector<char>, std::vector<uint32_t>> vertex_shader_code;
        std::variant<std::vector<char>, std::vector<uint32_t>> fragment_shader_code;
        std::vector<VertexBinding> vertex_binding;
        std::vector<VertexInput> vertex_formats;
        std::vector<BindingLayout*> binding_layout;
        std::vector<PushConstant> push_constant_layout;
        enum class VertexTopology {Triangle_List, Triangle_Strip} topology;
        enum class CullMode {Front, Back, None} cull_mode;
        std::optional<std::tuple<std::vector<ColorBlend>, Vec4<float>>> color_blend;
        std::optional<DepthStencilInfo> depth_stencil;
        std::variant<RenderingInfo, RenderPassInfo> render_mode;
    };
    struct ComputePipelineInfo {
        std::variant<std::vector<char>, std::vector<uint32_t>> compute_shader_code;
        std::vector<BindingLayout*> binding_layout;
        std::vector<PushConstant> push_constant_layout;
    };
    /* input */
    enum class Key : uint8_t
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
        Menu, Escape,
        EndOfEnum
    };
    enum class MouseKey : uint8_t
    {
        Unknown = 0,
        BtnLeft, BtnRight, BtnMiddle,
        EndOfEnum
    };
    enum class MouseInputMode : uint8_t { show, hide, disable };
    /* memory */
    enum class BufferUsage : uint32_t
    {
        copy_src  = 1U << 0,
        copy_dst  = 1U << 1,
        vertex = 1U << 2,
        index  = 1U << 3,
        uniform = 1U << 4,
        storage = 1U << 5,
        command = 1U << 6,
    };
    using BufferUsageMask = BitFlags<BufferUsage>;
    enum class InputUintType { u8, u16, u32 };
    struct ImageInfo { Vec2<uint32_t> size; ImageFormat format; size_t depth; void* data;};
    
    /* binding */
    enum class GPUBindingDataType { uniform, storage };
    enum class BindingType { uniform, uniform_dynamic, uniform_texel, storage, storage_dynamic, storage_texel, image_sampler, sampled_image };
    struct BindingInfo { uint32_t binding_index; uint32_t binding_array_count; ShaderType shader_type; BindingType binding_type; };

}

/*-------------------------------------------------
 | ▒██     ██░ █████████ ░██  ███        ███████  |
 | ▒██     ██░    ███    ░██  ███       ██    ███ |
 | ▒██     ██░    ███    ░██  ███       █████▓    |
 | ▒██     ██░    ███    ░██  ███          ░█████ |
 |  ██░   ░██     ███    ░██  ███      ███     ██ |
 |   ███████      ███    ░██  █████████ ▒███████  |
--------------------------------------------------*/
namespace PWEngine::GPU 
{
    std::optional<std::variant<std::vector<char>, std::vector<uint32_t>>> compilerShader(const std::string& source, ShaderType type, const std::string& name);
    std::optional<std::variant<std::vector<char>, std::vector<uint32_t>>> openShader(std::string file_path);

}

/*------------------------------------------
 | ░██  ████     ████  ████████   ██▓      |
 | ░██  █████   █████  ███    ██  ██▓      |
 | ░██  ██████  ██▓██  ███   ░██  ██▓      |
 | ░██  ███ ██ ██ ▓██  ████████   ██▓      |
 | ░██  ███ ░████ ▓██  ███        ██▓      |
 | ░██  ███  ███  ▓██  ███        ████████ |
-------------------------------------------*/
namespace PWEngine::GPU
{

/* -------- */
/* instance */
/* -------- */
    struct Input
    {
        std::atomic<bool> window_should_close{};
        std::array<std::atomic<bool>, 255> key_list{};
        std::atomic<double> mouse_position_x{};
        std::atomic<double> mouse_position_y{};
        std::array<std::atomic<bool>, 8> mouse_key{};
        std::atomic<bool> is_hover{};
    };

    class GPUInstance
    {
    private:
        void createInstance(GPUInstanceInfo info);
        void createDevice();
        void createCommandPool();
        void updateInput();

        bool is_debug;
        std::unordered_map<size_t, WindowTask*> gpu_tasks;
        mutable std::timed_mutex create_mtx;
        mutable std::timed_mutex task_mtx;

        mutable std::timed_mutex visit_mtx;
        
        std::atomic<bool> is_stop = false;
        std::atomic<bool> is_use_sumbit = false;

        uint64_t task_index = 0;
        std::atomic<uint64_t> live_task;
    public:
        LogSystem* log;
        struct Impl;
        std::unique_ptr<Impl> self;

        Input input;
        
        GPUInstance(LogSystem* log, GPUInstanceInfo info);
        void run(uint64_t main_task_id);
        void launchTask(uint64_t task_id);

        Window* focused_window = nullptr;
        void setMouseCursor(MouseInputMode mode);
        template<class T, class... Args> requires std::derived_from<T, WindowTask>
        uint64_t createWindowTask(Args&&... args)
        {
            std::unique_lock<std::timed_mutex> lk(create_mtx, std::defer_lock);
            bool got_lock = false;
            while (!got_lock && !is_stop.load()) {
                got_lock = lk.try_lock_for(std::chrono::milliseconds(1));
            }
            if (this->is_stop.load()) return UINT64_MAX;
            WindowTask* task = new T(std::forward<Args>(args)...);
            this->gpu_tasks.insert({this->task_index, task});
            ++this->task_index;
            return task_index - 1;
        }
        template<typename F> void mutexVisit(F&& func)
        {
            std::unique_lock<std::timed_mutex> lk(this->visit_mtx, std::defer_lock);
            bool acquired = false;
            while (!acquired && !is_stop.load())
            {
                acquired = lk.try_lock_for(std::chrono::milliseconds(1));
                if (!acquired) continue;
                if (this->is_use_sumbit.load())
                {
                    lk.unlock();
                    acquired = false;
                }
            }
            if (!acquired || is_stop.load()) return;

            this->is_use_sumbit = true;
            func();
            this->is_use_sumbit = false;
        }
        ~GPUInstance();
    };
    
/* --------- */
/* allocator */
/* --------- */
    struct IGPUBuffer
    {
        struct Impl;
        std::unique_ptr<Impl> self;
        IGPUBuffer();
    };

    struct GPULocalBuffer : public IGPUBuffer
    {
        GPUInstance* p_instance;
        GPULocalBuffer(GPUInstance* instance, BufferUsageMask usage, size_t size, void* data);
        ~GPULocalBuffer();
    };

    struct GPUStagingBuffer : public IGPUBuffer
    {
        GPUInstance* p_instance;
        void* map_ptr;
        GPUStagingBuffer(GPUInstance* instance, BufferUsageMask usage, size_t size, void* data);
        void readData(void* data, size_t offset, size_t size);
        void writeData(void* data, size_t offset, size_t size);
        ~GPUStagingBuffer();
    };

    struct GPUBindingBuffer : public IGPUBuffer
    {
        void* map_pointer;
        GPUInstance* p_instance;
        GPUBindingBuffer(GPUInstance* instance, BufferUsageMask usage, size_t size, void* data);
        ~GPUBindingBuffer();
    };

    struct GPUImage
    {
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        GPUImage(GPUInstance* instance, File::ImageRGBA8* image, bool is_SRGB, bool is_use_mipmap);
        GPUImage(GPUInstance* instance, ImageInfo image_info, bool is_use_mipmap);
        ~GPUImage();
    };

    class GPUImageView
    {
    private:
        void createTextureView();
        void createTextureSampler();
    public:
        GPUImage* p_image;
        struct Impl;
        std::unique_ptr<Impl> self;
        GPUImageView(GPUImage* image);
        ~GPUImageView();
    };

/* -------- */
/* pipeline */
/* -------- */
    struct IPipeline
    {
        GPUInstance* p_instance;
        enum class PipelineType {Render, Compute} type;
        struct Impl;
        std::unique_ptr<Impl> self;
        IPipeline();
    };

    struct RenderPipeline : public IPipeline
    {
        void builder(RenderPipelineInfo info);
        /* Constructor */
        RenderPipeline(GPUInstance* instance, RenderPipelineInfo info);
        ~RenderPipeline();
    };

    struct ComputePipeline : public IPipeline
    {   
        void builder(ComputePipelineInfo info);
        /* Constructor */
        ComputePipeline(GPUInstance* instance, ComputePipelineInfo info);
        ~ComputePipeline();
    };

/* ------- */
/* binding */
/* ------- */
    class BindingLayout
    {
    private:
        void createPool();
        void createLayout();
    public:
        std::vector<BindingInfo> binding_infos;
        std::array<size_t, 8> binding_type_count;
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        BindingLayout(GPUInstance* instance, std::vector<BindingInfo> binding_info);
        std::unique_ptr<Binding> createBinding();
        ~BindingLayout();
    };  

    struct Binding
    {
        struct Impl;
        std::unique_ptr<Impl> self;
        BindingLayout* p_layout;
        Binding(BindingLayout* layout);
        ~Binding();
        void update(uint32_t start_offset, std::vector<std::variant<GPUBindingBuffer*, GPUImageView*>> objects);
    };

/* ---- */
/* task */
/* ---- */

    class GPUTask
    {
    private:
        void waitFenceInProcess();
        void submitCommandInProcess();
        bool task(double delta);
    public:
        GPUTask(GPUInstance* instance);
        ~GPUTask();
    };

    struct RenderingCommand
    {
        RenderingCommand(RecordCommand* command): p_command(command){}
        RecordCommand* p_command;
        void bindVertices(std::vector<IGPUBuffer*> buffers, std::vector<uint64_t> offset);
        void bindIndex(IGPUBuffer* buffer, InputUintType size);
        void draw(uint64_t vertex_count, uint64_t instance_count, uint64_t vertex_offset, uint64_t instance_offset);
    };

    class RecordCommand
    {
    private:
        void BeginRendering(Vec4<float> color, bool is_clear_background, WindowTask* task);
        void EndRendering(WindowTask* task);
        void BeginRenderPass();
        void EndRenderingPass();
    public:
        RecordCommand(WindowTask* task): p_task(task){};
        WindowTask* p_task;
        void bindPipeline(IPipeline* pipeline);
        void bindBinding(IPipeline* pipeline, std::vector<Binding*> binding);
        void pushConstant(IPipeline* pipeline, size_t offset, size_t data_size, void* data, ShaderType type);
        void setViewPort(Vec2<int32_t> offset, Vec2<uint32_t> window_size, Vec2<float> depth_area);
        void setScissor(Vec2<int32_t> offset, Vec2<uint32_t> window_size);
        void Dispatch(Vec3<uint32_t> group_size);
        template<std::invocable<RenderingCommand*> F> void Rendering(Vec4<float> color, bool is_clear_background, WindowTask* task, F&& func)
        {
            RenderingCommand rendering(this);
            this->BeginRendering(color, is_clear_background, task);
            func(&rendering);
            this->EndRendering(task);
        }
    };

/* ------ */
/* window */
/* ------ */

    class Window
    {
    private:
        void createWindow(WindowInfo info);
        void createSwapchain(Vec2<uint32_t>& swapchain_extent);
        void createDepth(Vec2<uint32_t> swapchain_extent);
        void createImageView();
        void recreateSwapchain();
    public:
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        Input* input;
        Window(GPUInstance* instance, WindowInfo info);
        Format getFormat();
        Vec2<uint32_t> getWindowSize();
        bool checkIsHoverScreen();
        ~Window();
        friend class WindowTask;
    };

    class WindowTask
    {
    private:
        void createSync();
        void createCommandPool();

        void beginCommandInProcess();
        void endCommandInProcess();

        void waitFenceInProcess(uint32_t* swapchain_image_index);
        void submitCommandInProcess(uint32_t* swapchain_image_index);

    public:
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        std::unique_ptr<Window> m_window;

        uint8_t flight_fences_index;
        uint32_t swapchain_image_index;

        bool task_loop(double delta);
        virtual bool process(double delta) = 0;

        template<std::invocable<RecordCommand*> F>
        void recordCommand(F&& func)
        {
            RecordCommand command(this);
            this->beginCommandInProcess();
            func(&command);
            this->endCommandInProcess();
        }
        WindowTask(GPUInstance* instance, WindowInfo info);
        virtual ~WindowTask();
    };


}

