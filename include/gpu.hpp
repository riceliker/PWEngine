// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
#pragma once
#include "math.hpp"
#include "stream.hpp"

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <optional>
#include <unordered_map>
#include <vector>

#include <variant>


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
    class GPUInstance; class Input; class IGPUTask; 
    /* allocator */
    class GPUBuffer; class GPUImage;
    /* pipeline */
    class Pipeline; class GPUTask;
    /* window */
    class Window; class GPUTaskWithWindow; class RenderingCommand; class RecordCommandWithWindow;
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
    enum class ImageFormat { RGBA8, RGB8, D_F32_S_U8 };
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

    enum class DescriptorType { Uniform, Sampler, Storage };
    enum class ShaderType { Vertex, Fragment, Compute };
    struct RenderShaderPath { std::string vertex_shader; std::string fragment_shader; };
    struct ShaderLayoutInfo { uint32_t binding_index; uint32_t binding_array_count; DescriptorType descriptor_type; ShaderType shader_type; };
    struct PushConstant { uint32_t offset; uint32_t size; };
    struct VertexBinding { uint32_t binding; uint32_t size; };
    struct VertexInput { uint32_t binding; uint32_t location; uint32_t offset; enum class Type { u8, i32, f32 } type; enum class Length { vec2, vec3, vec4 } len; };
    enum class BlendOp { add, src_minus_dst, dst_minus_src, min, max };
    enum class BlendObject { zero, one, src_color, one_minus_src_color, dst_color, one_minus_dst_color, src_alpha, one_minus_src_alpha, dst_alpha, one_minus_dst_alpha, constant_color, one_minus_constant_color, constant_alpha, one_minus_constant_alpha, };
    struct ColorBlend { BlendOp color_op; BlendObject color_src; BlendObject color_dst; BlendOp alpha_op; BlendObject alpha_src; BlendObject alpha_dst; Vec4<bool> write_mask;};
    struct DepthStencilInfo { };
    struct RenderingInfo { };
    struct RenderPassInfo { };
    struct PipelineInfo {
        std::variant<std::vector<char>, std::vector<uint32_t>> vertex_shader_code;
        std::variant<std::vector<char>, std::vector<uint32_t>> fragment_shader_code;
        std::vector<VertexBinding> vertex_bindings;
        std::vector<VertexInput> vertex_formats;
        std::optional<std::vector<std::vector<ShaderLayoutInfo>>> descriptor_set;
        std::optional<std::vector<PushConstant>> push_constant_layout;
        enum class VertexTopology {Triangle_List, Triangle_Strip} topology;
        enum class CullMode {Front, Back, None} cull_mode;
        std::optional<std::vector<ColorBlend>> color_blend;
        Vec4<float> color_blend_constant;
        std::optional<DepthStencilInfo> depth_stencil;
        std::variant<RenderingInfo, RenderPassInfo> render_mode;
    };
    enum class PipelineType {Render, Compute};
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
    enum MouseKey : uint8_t
    {
        Unknown = 0,
        BtnLeft, BtnRight, BtnMiddle,
        EndOfEnum
    };

    enum class InputUintType { u8, u16, u32 };
    enum class GPUBufferType { vertex, index };
    
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
struct IRecordCommand
{
    IRecordCommand(IGPUTask* task): p_task(task){};
    IGPUTask* p_task;
    virtual void bindPipeline(Pipeline* pipeline, PipelineType type);
};

class IGPUTask
    {
    private:
        void createSync();
        void createCommandPool();
    protected:
        void beginCommandInProcess();
        void endCommandInProcess();
        virtual bool sub_thread_loop(double delta) = 0;
        const uint8_t flight_fences_index_counts = 3;
        IGPUTask(GPUInstance* instance);
    public:
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        uint8_t flight_fences_index;
        virtual bool loop(double delta) = 0;
        virtual ~IGPUTask();
        
        friend class GPUInstance;
    };

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
        std::unordered_map<size_t, std::unique_ptr<IGPUTask>> gpu_tasks;
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
        void onFocus(Window* win, int focused);

        template<class T, class... Args> requires std::derived_from<T, IGPUTask>
        uint64_t createGPUTask(Args&&... args)
        {
            std::unique_lock<std::timed_mutex> lk(create_mtx, std::defer_lock);
            bool got_lock = false;
            while (!got_lock && !is_stop.load()) {
                got_lock = lk.try_lock_for(std::chrono::milliseconds(1));
            }
            if (this->is_stop.load()) return UINT64_MAX;
            std::unique_ptr<IGPUTask> task = std::make_unique<T>(std::forward<Args>(args)...);
            this->gpu_tasks.insert({this->task_index, std::move(task)});
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
    class GPUBuffer
    {
    private:
        void createGPUBuffer(size_t element_total_size, void* element, GPUBufferType type);
    public:
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        GPUBuffer(GPUInstance* p_instance, GPUBufferType type, size_t byte_size, void* data);
        template<typename T> static std::unique_ptr<GPUBuffer> builderUnique(GPUInstance* p_instance, GPUBufferType type, std::vector<T> element)
        {
            return std::make_unique<GPUBuffer>(p_instance, type, element.size() * sizeof(T), element.data());
        }
        template<typename T> static std::shared_ptr<GPUBuffer> builderShared(GPUInstance* p_instance, GPUBufferType type, std::vector<T> element)
        {
            return std::make_shared<GPUBuffer>(p_instance, type, element.size() * sizeof(T), element.data());
        }
        ~GPUBuffer();
    };

    class GPUImage
    {
    private:
        void createGPUImage(Vec2<uint32_t> image_size, ImageFormat format, size_t image_depth, void* element);
    public:
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        template<typename T>
        GPUImage(GPUInstance* p_instance, Vec2<uint32_t> image_size, ImageFormat format, std::vector<T> pixels)
        {
            this->p_instance = p_instance;
            createGPUImage(image_size, sizeof(T), format, pixels.data());
        }
        ~GPUImage();
    };

/* -------- */
/* pipeline */
/* -------- */

    class Pipeline
    {
    public:
        GPUInstance* p_instance;
        struct Impl;
        std::unique_ptr<Impl> self;
        void builder(PipelineInfo info);
        /* Constructor */
        Pipeline(GPUInstance* instance, PipelineInfo info);
        ~Pipeline();
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
        ~Window();
        friend class GPUTaskWithWindow;
    };

/* ---- */
/* task */
/* ---- */
    struct RecordCommand
    {
       
    };

    class GPUTask : public IGPUTask
    {
    private:
        void waitFenceInProcess();
        void submitCommandInProcess();
        bool sub_thread_loop(double delta) override;
        GPUTask(GPUInstance* instance);
        ~GPUTask();
    };

    class RenderingCommand
    {
    private:
    public:
        RenderingCommand(RecordCommandWithWindow* command): p_command(command){}
        RecordCommandWithWindow* p_command;
        void bindVertices(std::vector<GPUBuffer*> buffers, std::vector<uint64_t> offset);
        void bindIndex(GPUBuffer* buffer, InputUintType size);
        void draw(uint64_t vertex_count, uint64_t instance_count, uint64_t vertex_offset, uint64_t instance_offset);
    };

    class RecordCommandWithWindow
    {
    private:
        void BeginRendering(Vec4<float> color, bool is_clear_background);
        void EndRendering();
        void BeginRenderPass();
        void EndRenderingPass();
    public:
        RecordCommandWithWindow(GPUTaskWithWindow* task): p_task(task){};
        GPUTaskWithWindow* p_task;
        void bindPipeline(Pipeline* pipeline, PipelineType type);
        void setViewPort();
        void setScissor();
        template<std::invocable<RenderingCommand&> F> void Rendering(Vec4<float> color, bool is_clear_background, F&& func)
        {
            RenderingCommand rendering(this);
            this->BeginRendering(color, is_clear_background);
            func(rendering);
            this->EndRendering();
        }
    };

    class GPUTaskWithWindow : public IGPUTask
    {
    private:
        void waitFenceInProcess(uint32_t* swapchain_image_index);
        void submitCommandInProcess(uint32_t* swapchain_image_index);
        bool sub_thread_loop(double delta) override;
    public:
        std::unique_ptr<Window> m_window;
        uint32_t swapchain_image_index;
        template<std::invocable<RecordCommandWithWindow*> F>
        void recordCommand(F&& func)
        {
            RecordCommandWithWindow command(this);
            this->beginCommandInProcess();
            func(&command);
            this->endCommandInProcess();
        }
        GPUTaskWithWindow(GPUInstance* instance, WindowInfo info);
        ~GPUTaskWithWindow();
    };


}

