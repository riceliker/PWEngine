#include "core/render.hpp"
#include "core/render/_render.hpp"
#include "shaderc/shaderc.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace PWEngine::Render 
{
    std::unique_ptr<PipelineHUD> RenderContext::createPipelineHUD()
    {
        std::unique_ptr<PipelineHUD> obj = std::make_unique<PipelineHUD>(this);        
        return obj;
    }

    static inline std::optional<std::vector<uint32_t>> compilerShader(const std::string& source, const shaderc_shader_kind kind, const std::string& name)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
        options.SetSourceLanguage(shaderc_source_language_glsl);

        const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, name.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) 
        {
            printf("%s", result.GetErrorMessage().c_str());
            return std::nullopt;
        }

        std::vector<uint32_t> spv = {result.begin(), result.end()};

        return spv;
    }

    static inline std::optional<VkShaderModule> createShaderModule(const std::vector<uint32_t>& code, VkDevice device) {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = 4 * code.size();
        create_info.pCode = code.data();

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shaderModule) != VK_SUCCESS) {
            return std::nullopt;
        }

        return shaderModule;
    }

    static inline 

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
            out_color = texture(texture_sampler, frag_uv);
        }
    )";

    PipelineHUD::PipelineHUD(RenderContext* super) : self(std::make_unique<Impl>())
    {
        this->p_context = super;

        /* Texture Layout */
        VkDescriptorSetLayout descriptor_set_layout;
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings = &binding;

        if (vkCreateDescriptorSetLayout(this->p_context->m_device->device, &layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS)
            sendLog(this->p_context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create descriptor set layout!");  
        
        /* Push Constant */
        VkPushConstantRange hud_push_range{};
        hud_push_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        hud_push_range.offset = 0;
        hud_push_range.size = sizeof(NodeHUD);
        
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts = &descriptor_set_layout;
        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pPushConstantRanges = &hud_push_range;

        VkPipelineLayout pipeline_layout;
        if (vkCreatePipelineLayout(this->p_context->m_device->device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
            sendLog(this->p_context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create pipeline layout!");
        }
        
        /* ----- shader ----- */
        auto vertex_shader_code = compilerShader(vertex_shader, shaderc_glsl_vertex_shader, "main").value();
        auto fragment_shader_code = compilerShader(fragment_shader, shaderc_glsl_fragment_shader, "main").value();

        VkShaderModule vertShaderModule = createShaderModule(vertex_shader_code, this->p_context->m_device->device).value();
        VkShaderModule fragShaderModule = createShaderModule(fragment_shader_code, this->p_context->m_device->device).value();

        VkPipelineShaderStageCreateInfo vertex_shader_stage_info{};
        vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_info.module = vertShaderModule;
        vertex_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo fragment_shader_stage_info{};
        fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_shader_stage_info.module = fragShaderModule;
        fragment_shader_stage_info.pName = "main";

        std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {vertex_shader_stage_info, fragment_shader_stage_info};
        
        /* ----- Input ----- */
        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 0;
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        vertex_input_info.pVertexBindingDescriptions = nullptr;
        vertex_input_info.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        /* ----- State ----- */
        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();

        /* ----- rasterizer ----- */
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; /* default */
        rasterizer.rasterizerDiscardEnable = VK_FALSE; /* default: It must be render */
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL; /* default */
        rasterizer.lineWidth = 1.0f; /* default */
        rasterizer.cullMode = VK_CULL_MODE_NONE; /* default: backend face must be ignored */
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; /* default */
        rasterizer.depthBiasEnable = VK_FALSE; /* dynamic: see setDepthBias() */

        /* ----- multisample ----- */
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        /* ----- color ----- */
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        /* ----- create ----- */
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineRenderingCreateInfo rendering_create_info{};
        rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_create_info.colorAttachmentCount = 1;
        rendering_create_info.pColorAttachmentFormats = &this->p_context->m_swapchain->swapchain_image_format;
        rendering_create_info.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        rendering_create_info.stencilAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        
        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext = &rendering_create_info;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages.data();
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &inputAssembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pColorBlendState = &colorBlending;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = pipeline_layout;
        pipeline_info.renderPass = VK_NULL_HANDLE;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_info.pDepthStencilState = &depthStencil;

        VkPipeline graphics_pipeline;
        if (vkCreateGraphicsPipelines(this->p_context->m_device->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS)
            sendLog(this->p_context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create graphics pipeline!");

        for (auto shader_module : shader_stages)
        {
            vkDestroyShaderModule(this->p_context->m_device->device, shader_module.module, nullptr);
        }

        self->graphics_pipeline = graphics_pipeline;
        self->pipeline_layout = pipeline_layout;
        self->descriptor_set_layout = descriptor_set_layout;
    }

    PipelineHUD::~PipelineHUD()
    {
        vkDestroyPipeline(this->p_context->m_device->device, this->self->graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(this->p_context->m_device->device, this->self->pipeline_layout, nullptr);
        vkDestroyDescriptorSetLayout(this->p_context->m_device->device, this->self->descriptor_set_layout, nullptr);
    } 
}