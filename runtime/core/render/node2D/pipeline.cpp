#include "core/render.hpp"
#include "core/render/_render.hpp"
#include "core/render/_utils.hpp"
#include "shaderc/shaderc.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace PWEngine::Render 
{
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
        create_info.codeSize = code.size();
        create_info.pCode = code.data();

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shaderModule) != VK_SUCCESS) {
            return std::nullopt;
        }

        return shaderModule;
    }

    const std::string vertex_shader = R"(
        #version 450
        layout(set = 0, binding = 0) uniform Camera2D {
            mat4 view;
        } camera;

        layout(location = 0) in vec2 in_position;
        layout(location = 1) in vec4 in_color;
        layout(location = 2) in vec2 in_uv;

        layout(location = 0) out vec4 frag_color;
        layout(location = 1) out vec2 frag_uv;

        void main() {
            gl_Position = camera.view * vec4(in_position, 0.0, 1.0);
            frag_color = in_color;
            frag_uv = in_uv;
        }
    )";

    const std::string fragment_shader = R"(
        #version 450
        layout(set = 1, binding = 0) uniform sampler2D texture_sampler;

        layout(location = 0) in vec4 frag_color;
        layout(location = 1) in vec2 frag_uv;

        layout(location = 0) out vec4 out_color;

        void main() {
            out_color = frag_color * texture(texture_sampler, frag_uv);
        }
    )";

    Pipeline2D::Pipeline2D(RenderContext* super) : self(std::make_unique<Impl>())
    {
        this->p_context = super;
        /* ----- descriptor ----- */
        std::vector<std::vector<ShaderLayoutInfo>> infos;

        /* Mesh2D set = 0 */
        infos.push_back({
            {0, 1, DescriptorType::Storage, ShaderType::Vertex}
        });
        /* Camera2D set = 1 */
        infos.push_back({
            {0, 1, DescriptorType::Uniform, ShaderType::Vertex}
        });

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
        std::vector<std::vector<VkDescriptorSet>> descriptor_sets;
        for (const auto layout : infos)
        {
            /* layout */
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
                }

                layout_bindings.push_back(binding);
            }

            VkDescriptorSetLayoutCreateInfo layout_info{};
            layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layout_info.bindingCount = static_cast<uint32_t>(layout_bindings.size());
            layout_info.pBindings = layout_bindings.data();

            if (vkCreateDescriptorSetLayout(this->p_context->m_device->device, &layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS)
                Stream::log(this->p_context->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create descriptor set layout!");
            descriptor_set_layouts.push_back(descriptor_set_layout);            
        }
        
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
        pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();

        VkPipelineLayout pipeline_layout;
        if (vkCreatePipelineLayout(this->p_context->m_device->device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
            Stream::log(this->p_context->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create pipeline layout!");
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
        auto bind = getBindingDescription<Vertex2D>();
        auto attribute = getAttributeDescriptions<Vertex2D>();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute.size());
        vertexInputInfo.pVertexBindingDescriptions = &bind;
        vertexInputInfo.pVertexAttributeDescriptions = attribute.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
        rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT; /* default: backend face must be ignored */
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
        pipeline_info.pVertexInputState = &vertexInputInfo;
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
        pipeline_info.pDepthStencilState = VK_NULL_HANDLE;

        VkPipeline graphics_pipeline;
        if (vkCreateGraphicsPipelines(this->p_context->m_device->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS)
            Stream::log(this->p_context->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create graphics pipeline!");

        for (auto shader_module : shader_stages)
        {
            vkDestroyShaderModule(this->p_context->m_device->device, shader_module.module, nullptr);
        }

        self->graphics_pipeline = graphics_pipeline;
        self->pipeline_layout = pipeline_layout;
        self->descriptor_set_layouts = std::move(descriptor_set_layouts);
    }

    Pipeline2D::~Pipeline2D()
    {
        vkDestroyPipeline(this->p_context->m_device->device, this->self->graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(this->p_context->m_device->device, this->self->pipeline_layout, nullptr);
        for (const auto& layout : this->self->descriptor_set_layouts)
        {
            vkDestroyDescriptorSetLayout(this->p_context->m_device->device, layout, nullptr);
        }
    } 
}