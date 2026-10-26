#include "core/render.hpp"
#include <optional>
#include <string>
#include <vector>

namespace PWEngine::Render 
{
    std::unique_ptr<PipelineHUD> RenderContext::createPipelineHUD()
    {
        std::unique_ptr<PipelineHUD> obj = std::make_unique<PipelineHUD>(this);        
        return obj;
    }

    const std::string vertex_shader = R"(
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

    const std::string fragment_shader = R"(
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D texture_sampler;

        layout(location = 0) in vec2 frag_uv;

        layout(location = 0) out vec4 out_color;

        void main() {
            vec4 color = texture(texture_sampler, frag_uv);
            out_color = color;
        }
    )";

    PipelineHUD::PipelineHUD(RenderContext* super) : Pipeline(super)
    {
        this->p_context = super;

        PipelineInfo info{};

        info.vertex_shader_code = std::move(compilerShader(vertex_shader, ShaderType::Vertex, "main").value());
        info.fragment_shader_code = std::move(compilerShader(fragment_shader, ShaderType::Fragment, "main").value());

        info.descriptor_set = {
            {{0, 1, DescriptorType::Sampler, ShaderType::Fragment}},
        };

        info.push_constant_layout = {
            {0, sizeof(NodeHUD)}
        };

        info.vertex_bindings = {};
        info.vertex_formats = {};

        info.color_blend = {
            {
                BlendOp::add, 
                BlendObject::src_alpha,
                BlendObject::one_minus_src_alpha,
                BlendOp::add,
                BlendObject::one,
                BlendObject::zero,
                {true, true, true, true}
            }
        };
        info.color_blend_constant = {0, 0, 0, 0};

        info.topology = PipelineInfo::VertexTopology::Triangle_Strip;
        info.cull_mode = PipelineInfo::CullMode::None;
        info.depth_stencil = std::nullopt;
        info.render_mode = RenderingInfo{};

        this->builder(info);        
    }
}