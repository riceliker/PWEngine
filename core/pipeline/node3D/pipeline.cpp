#include "core/format.hpp"
#include "core/render.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace PWEngine::Render 
{
    std::unique_ptr<Pipeline3D> RenderContext::createPipeline3D(std::vector<std::vector<ShaderLayoutInfo>> infos_ext, ShaderPath path)
    {   
        std::unique_ptr<Pipeline3D> obj = std::make_unique<Pipeline3D>(this, path);        
        return obj;
    }

    Pipeline3D::Pipeline3D(RenderContext* super, ShaderPath path) : Pipeline(super)
    {
        PipelineInfo info{};

        info.vertex_shader_code = std::move(openShader(path.vertex_shader).value());
        info.fragment_shader_code = std::move(openShader(path.fragment_shader).value());

        info.descriptor_set = {
            {{0, 1, DescriptorType::Storage, ShaderType::Vertex}},
            {{0, 1, DescriptorType::Sampler, ShaderType::Fragment}},
            {{0, 1, DescriptorType::Uniform, ShaderType::Vertex}}
        };

        info.push_constant_layout = std::nullopt;
        
        info.vertex_bindings = {
            {0, sizeof(Vertex3D)}
        };

        info.vertex_formats = {
            {0, 0, offsetof(Vertex3D, position), VertexInput::Type::f32, VertexInput::Length::vec3},
            {0, 1, offsetof(Vertex3D, color), VertexInput::Type::f32, VertexInput::Length::vec3},
            {0, 2, offsetof(Vertex3D, uv), VertexInput::Type::f32, VertexInput::Length::vec2}
        };

        info.color_blend = std::nullopt;
        info.color_blend_constant = {0, 0, 0, 0};

        info.topology = PipelineInfo::VertexTopology::Triangle_List;
        info.cull_mode = PipelineInfo::CullMode::Front;
        info.depth_stencil = DepthStencilInfo{};
        info.render_mode = RenderingInfo{};

        this->builder(info);
    }

}