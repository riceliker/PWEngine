#include "pipeline.hpp"
#include "gpu.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace PWEngine::Pipeline
{
    const std::string vertex_shader_code = R"(
        #version 450
        layout(push_constant) uniform HUDQuad {
            vec4 pos_rect;
            vec4 uv_rect;
            mat4 vp;
        } pc;

        layout(location=0) out vec2 frag_uv;

        void main() {
            vec2 pos[4] = vec2[4](
                vec2(pc.pos_rect.x, pc.pos_rect.y),
                vec2(pc.pos_rect.x + pc.pos_rect.z, pc.pos_rect.y),
                vec2(pc.pos_rect.x, pc.pos_rect.y - pc.pos_rect.w),
                vec2(pc.pos_rect.x + pc.pos_rect.z, pc.pos_rect.y - pc.pos_rect.w)
                
            );
            vec2 p = pos[gl_VertexIndex];
            gl_Position = pc.vp * vec4(p, 0, 1.0);

            vec2 uv[4] = vec2[4](
                vec2(pc.uv_rect.x, pc.uv_rect.y),
                vec2(pc.uv_rect.z, pc.uv_rect.y),
                vec2(pc.uv_rect.x, pc.uv_rect.w),
                vec2(pc.uv_rect.z, pc.uv_rect.w)
            );
            frag_uv = uv[gl_VertexIndex];
        }
    )";

    const std::string fragment_shader_code = R"(
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D texture_sampler;

        layout(location = 0) in vec2 frag_uv;

        layout(location = 0) out vec4 out_color;

        void main() {
            vec4 color = texture(texture_sampler, frag_uv);
            out_color = color;
        }
    )";

    RectPipeline::RectPipeline(GPU::GPUInstance* instance)
    {
        std::vector<GPU::BindingInfo> texture_binding_info = {
            {0, 1, GPU::ShaderType::Fragment, GPU::BindingType::image_sampler}
        };
        this->set_texture = std::make_unique<GPU::BindingLayout>(instance, texture_binding_info);

        GPU::PipelineInfo pipeline_info = {
            PWEngine::GPU::compilerShader(vertex_shader_code, PWEngine::GPU::ShaderType::Vertex, "main").value(),
            PWEngine::GPU::compilerShader(fragment_shader_code, PWEngine::GPU::ShaderType::Fragment, "main").value(),
            {{0,sizeof(UnlitVertex)}},
            {
                {0, 0, offsetof(UnlitVertex, position), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec3},
                {0, 1, offsetof(UnlitVertex, uv), PWEngine::GPU::VertexInput::Type::f32, PWEngine::GPU::VertexInput::Length::vec2},
            },
            {set_texture.get()},
            {{}},
            PWEngine::GPU::PipelineInfo::VertexTopology::Triangle_List,
            PWEngine::GPU::PipelineInfo::CullMode::None,
            std::nullopt,
            std::nullopt,
            PWEngine::GPU::RenderingInfo{},
        };
        this->p_pipeline = std::make_unique<GPU::Pipeline>(instance, pipeline_info);
    

    }
}