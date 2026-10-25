#include "gpu.hpp"
#include "gpu/instance/_instance.hpp"
#include "_pipeline.hpp"
#include "gpu/_format.hpp"

#include "stream.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
namespace PWEngine::GPU
{
    IPipeline::IPipeline(GPUInstance* super) : self(std::make_unique<Impl>())
    {
        this->p_instance= super;
    }
    static inline VkBlendFactor ToVulkanEnumBlendFactor(BlendObject obj)
    {   
        switch (obj) 
        {
        case BlendObject::zero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendObject::one:
            return VK_BLEND_FACTOR_ONE;
        case BlendObject::src_color:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendObject::one_minus_src_color:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendObject::dst_color:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendObject::one_minus_dst_color:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendObject::src_alpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendObject::one_minus_src_alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendObject::dst_alpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendObject::one_minus_dst_alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendObject::constant_color:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendObject::one_minus_constant_color:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendObject::constant_alpha:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendObject::one_minus_constant_alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        }
    }
    static inline VkBlendOp ToVulkanEnumBlendOp(BlendOp op)
    {
        switch (op) 
        {
        case BlendOp::add:
            return VK_BLEND_OP_ADD;
        case BlendOp::src_minus_dst:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOp::dst_minus_src:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::min:
            return VK_BLEND_OP_MIN;
        case BlendOp::max:
            return VK_BLEND_OP_MAX;
        }
    }
    void IPipeline::builder(PipelineInfo info)
    {
        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        /* --- Shader Module --- */
        std::optional<VkShaderModule> vert_shader_module_maybe = createShaderModule(info.vertex_shader_code, this->p_instance->self->device);
        std::optional<VkShaderModule> frag_shader_module_maybe = createShaderModule(info.fragment_shader_code, this->p_instance->self->device);

        if (!vert_shader_module_maybe.has_value()) 
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "Can not read Vertex Shader.");
        if (!frag_shader_module_maybe.has_value())
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "Can not read Fragment Shader.");

        VkPipelineShaderStageCreateInfo vertex_shader_stage_info{};
        vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_info.module = vert_shader_module_maybe.value();
        vertex_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo fragment_shader_stage_info{};
        fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_shader_stage_info.module = frag_shader_module_maybe.value();
        fragment_shader_stage_info.pName = "main";

        std::vector<VkPipelineShaderStageCreateInfo> shader_module = {vertex_shader_stage_info, fragment_shader_stage_info};
        pipeline_info.stageCount = shader_module.size();
        pipeline_info.pStages = shader_module.data();

        /* --- Vertex Layout --- */
        std::vector<VkVertexInputBindingDescription> binding;
        for (const auto& vertex_binding : info.vertex_bindings)
        {
            VkVertexInputBindingDescription binding_description{};
            binding_description.binding = vertex_binding.binding;
            binding_description.stride = vertex_binding.size;
            binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            binding.push_back(binding_description);
        }

        std::vector<VkVertexInputAttributeDescription> attribute{};
        for (const auto& vertex_format : info.vertex_formats)
        {
            VkVertexInputAttributeDescription attribute_description;
            attribute_description.binding = vertex_format.binding;
            attribute_description.location = vertex_format.location;
            attribute_description.offset = vertex_format.offset;
            switch (vertex_format.type)
            {
            case VertexInput::Type::u8:
                switch (vertex_format.len) 
                {
                case VertexInput::Length::vec2:
                    attribute_description.format = VK_FORMAT_R8G8_SNORM;
                break;
                case VertexInput::Length::vec3:
                    attribute_description.format = VK_FORMAT_R8G8B8_SNORM;
                break;
                case VertexInput::Length::vec4:
                    attribute_description.format = VK_FORMAT_R8G8B8A8_SNORM;
                break;
                }
            break;
            case VertexInput::Type::i32:
                switch (vertex_format.len) 
                {
                case VertexInput::Length::vec2:
                    attribute_description.format = VK_FORMAT_R32G32_SINT;
                break;
                case VertexInput::Length::vec3:
                    attribute_description.format = VK_FORMAT_R32G32B32_SINT;
                break;
                case VertexInput::Length::vec4:
                    attribute_description.format = VK_FORMAT_R32G32B32A32_SINT;
                break;
                }
            break;
            case VertexInput::Type::f32:
                switch (vertex_format.len) 
                {
                case VertexInput::Length::vec2:
                    attribute_description.format = VK_FORMAT_R32G32_SFLOAT;
                break;
                case VertexInput::Length::vec3:
                    attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
                case VertexInput::Length::vec4:
                    attribute_description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
                }
            break;
            }
            attribute.push_back(attribute_description);
        }
            
        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding.size());
        vertex_input_info.pVertexBindingDescriptions = (binding.size()!=0)? binding.data(): nullptr;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute.size());
        vertex_input_info.pVertexAttributeDescriptions = (attribute.size()!=0)? attribute.data(): nullptr;

        pipeline_info.pVertexInputState = &vertex_input_info;

        /* --- Buffer Layout --- */
        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
        if (info.descriptor_set.has_value())
        {
            for (const auto layout : info.descriptor_set.value())
            {
                VkDescriptorSetLayout descriptor_set_layout;
                
                std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
                for (const auto info : layout)
                {
                    VkDescriptorSetLayoutBinding binding;
                    binding.binding = info.binding_index;
                    binding.descriptorCount = info.binding_array_count;
                    switch (info.descriptor_type) 
                    {
                    case DescriptorType::Uniform:
                        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    break;
                    case DescriptorType::Sampler:
                        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        binding.pImmutableSamplers = nullptr;
                    break;
                    case DescriptorType::Storage:
                        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;
                    }
                    
                    switch (info.shader_type) 
                    {
                    case ShaderType::Vertex:
                        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                    break;
                    case ShaderType::Fragment:
                        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;
                    default:
                    break;
                    }

                    layout_bindings.push_back(binding);
                }

                VkDescriptorSetLayoutCreateInfo descriptor_set{};
                descriptor_set.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                descriptor_set.bindingCount = static_cast<uint32_t>(layout_bindings.size());
                descriptor_set.pBindings = layout_bindings.data();

                if (vkCreateDescriptorSetLayout(this->p_instance->self->device, &descriptor_set, nullptr, &descriptor_set_layout) != VK_SUCCESS)
                    sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create descriptor set layout!");
                descriptor_set_layouts.push_back(descriptor_set_layout);            
            }
        }

        std::vector<VkPushConstantRange> push_constant_ranges;
        if (info.push_constant_layout.has_value())
        {
            for (const auto& range : info.push_constant_layout.value())
            {
                VkPushConstantRange push_range{};
                push_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                push_range.offset = range.offset;
                push_range.size = range.size;
                push_constant_ranges.push_back(push_range);
            }
        }
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = descriptor_set_layouts.size();
        pipeline_layout_info.pSetLayouts = (descriptor_set_layouts.size() != 0)?descriptor_set_layouts.data(): nullptr;
        pipeline_layout_info.pushConstantRangeCount = push_constant_ranges.size();
        pipeline_layout_info.pPushConstantRanges = (push_constant_ranges.size() != 0)? push_constant_ranges.data(): nullptr;

        VkPipelineLayout pipeline_layout;
        if (vkCreatePipelineLayout(this->p_instance->self->device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create pipeline layout!");
        }
        this->self->pipeline_layout = pipeline_layout;
        this->self->descriptor_set_layouts = descriptor_set_layouts;
        pipeline_info.layout = pipeline_layout;

        /* --- Input Assembly --- */
        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        
        input_assembly.primitiveRestartEnable = VK_FALSE;
        switch (info.topology) 
        {
        case PipelineInfo::VertexTopology::Triangle_List:
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
        case PipelineInfo::VertexTopology::Triangle_Strip:
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        }
        pipeline_info.pInputAssemblyState = &input_assembly;

        /* --- Viewport State --- */
        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        pipeline_info.pViewportState = &viewport_state;

        /* --- Dynamic State --- */
        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();

        pipeline_info.pDynamicState = &dynamic_state;

        /* --- Rasterization --- */
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; /* default */
        rasterizer.rasterizerDiscardEnable = VK_FALSE; /* default: It must be render */
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL; /* default */
        rasterizer.lineWidth = 1.0f; /* default */
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; /* default */
        rasterizer.depthBiasEnable = VK_FALSE; /* dynamic: see setDepthBias() */
        switch (info.cull_mode) 
        {
        case PipelineInfo::CullMode::None:
            rasterizer.cullMode = VK_CULL_MODE_NONE;
        break;
        case PipelineInfo::CullMode::Front:
            rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
        break;
        case PipelineInfo::CullMode::Back:
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        break;
        }
        
        pipeline_info.pRasterizationState = &rasterizer;

        /* --- Multisample --- */
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        pipeline_info.pMultisampleState = &multisampling;

        /* --- Color Blending --- */
        std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments;
        if (info.color_blend.has_value())
        {
            for (const auto& level : info.color_blend.value())
            {
                VkPipelineColorBlendAttachmentState color_blend_attachment{};
                color_blend_attachment.blendEnable = VK_TRUE;

                color_blend_attachment.colorWriteMask = 
                    ((level.write_mask.x)? VK_COLOR_COMPONENT_R_BIT: 0) |
                    ((level.write_mask.y)? VK_COLOR_COMPONENT_G_BIT: 0) |
                    ((level.write_mask.z)? VK_COLOR_COMPONENT_B_BIT: 0) |
                    ((level.write_mask.w)? VK_COLOR_COMPONENT_A_BIT: 0) ;
                
                color_blend_attachment.colorBlendOp = ToVulkanEnumBlendOp(level.color_op);
                color_blend_attachment.srcColorBlendFactor = ToVulkanEnumBlendFactor(level.color_src);
                color_blend_attachment.dstColorBlendFactor = ToVulkanEnumBlendFactor(level.color_dst);

                color_blend_attachment.alphaBlendOp = ToVulkanEnumBlendOp(level.alpha_op);
                color_blend_attachment.srcAlphaBlendFactor = ToVulkanEnumBlendFactor(level.alpha_src);
                color_blend_attachment.dstAlphaBlendFactor = ToVulkanEnumBlendFactor(level.alpha_dst);

                color_blend_attachments.push_back(color_blend_attachment);
            }
        }
        else 
        {
            VkPipelineColorBlendAttachmentState color_blend_attachment{};
            color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            color_blend_attachment.blendEnable = VK_FALSE;
            color_blend_attachments.push_back(color_blend_attachment);
        }
        
        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount = color_blend_attachments.size();
        color_blending.pAttachments = color_blend_attachments.data();
        color_blending.blendConstants[0] = info.color_blend_constant.x;
        color_blending.blendConstants[1] = info.color_blend_constant.y;
        color_blending.blendConstants[2] = info.color_blend_constant.z;
        color_blending.blendConstants[3] = info.color_blend_constant.w;

        pipeline_info.pColorBlendState = &color_blending;

        /* --- Depth Stencil --- */
        VkPipelineDepthStencilStateCreateInfo depth_stencil{};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        if (info.depth_stencil.has_value())
        {
            depth_stencil.depthTestEnable = VK_TRUE;
            depth_stencil.depthWriteEnable = VK_TRUE;
            depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depth_stencil.depthBoundsTestEnable = VK_FALSE;
            depth_stencil.minDepthBounds = 0.0f; // Optional
            depth_stencil.maxDepthBounds = 1.0f; // Optional
            depth_stencil.stencilTestEnable = VK_FALSE;
            depth_stencil.front = {}; // Optional
            depth_stencil.back = {}; // Optional
        }
        else 
        {
            depth_stencil.depthTestEnable = VK_FALSE;
            depth_stencil.depthWriteEnable = VK_FALSE;
            depth_stencil.stencilTestEnable = VK_FALSE;
        }
        
        pipeline_info.pDepthStencilState = &depth_stencil;

        const auto srgb8 = VK_FORMAT_R8G8B8A8_SRGB;
        /* render mode */
        VkPipelineRenderingCreateInfo rendering{};
        std::visit([&](auto& arg) {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, RenderingInfo>) 
            {
                /* --- Rendering --- */
                rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
                rendering.colorAttachmentCount = 1;
                rendering.pColorAttachmentFormats = &info.format->format;
                rendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
                rendering.stencilAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

                pipeline_info.pNext = &rendering;
                pipeline_info.renderPass = VK_NULL_HANDLE;
                pipeline_info.subpass = 0;
            } 
            else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, RenderPassInfo>) 
            {
            
                
            }
        }, info.render_mode);
        

        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        VkPipeline graphics_pipeline;
        if (vkCreateGraphicsPipelines(this->p_instance->self->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create graphics pipeline!");

        for (auto shader_module : shader_module)
        {
            vkDestroyShaderModule(this->p_instance->self->device, shader_module.module, nullptr);
        }
        self->graphics_pipeline = graphics_pipeline;
    }
    IPipeline::~IPipeline()
    {
        vkDestroyPipeline(this->p_instance->self->device, this->self->graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(this->p_instance->self->device, this->self->pipeline_layout, nullptr);
        for (const auto& layout : this->self->descriptor_set_layouts)
        {
            vkDestroyDescriptorSetLayout(this->p_instance->self->device, layout, nullptr);
        }
    }
}