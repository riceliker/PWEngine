#include "render.hpp"
#include "file.hpp"
#include "impl.hpp"
#include "../context/impl.hpp"
#include "../node/impl.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::Render 
{
    Pipeline3D::Pipeline3D() : self(std::make_unique<Impl>())
    {

    }

    static inline std::optional<VkShaderModule> createShaderModule(const std::vector<char>& code, VkDevice device) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            return std::nullopt;
        }

        return shaderModule;
    }

    std::unique_ptr<Pipeline3D> RenderContext::createPipeline3D(std::vector<std::vector<ShaderLayoutInfo>> infos_ext, ShaderPath path)
    {   
        /* ----- descriptor ----- */
        std::vector<std::vector<ShaderLayoutInfo>> infos;

        /* Mesh3D set = 0 */
        infos.push_back({
            {0, 1, DescriptorType::Uniform, ShaderType::Vertex}
        });
        /* Material set = 1*/
        infos.push_back({
            {0, 1, DescriptorType::Sampler, ShaderType::Fragment}
        });
        /* Camera set = 2 */
        infos.push_back({
            {0, 1, DescriptorType::Uniform, ShaderType::Vertex}
        });

        for (const auto info_ext : infos_ext)
        {
            infos.push_back(info_ext);
        }

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

            if (vkCreateDescriptorSetLayout(this->m_device->device, &layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
            descriptor_set_layouts.push_back(descriptor_set_layout);            
        }
        
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
        pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();

        VkPipelineLayout pipeline_layout;
        if (vkCreatePipelineLayout(this->m_device->device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create pipeline layout!");
        }
        
        /* ----- shader ----- */
        auto vertex_shader_code = PWEngine::File::shaderReader(this->log, path.vertex_shader).value();
        auto fragment_shader_code = PWEngine::File::shaderReader(this->log, path.fragment_shader).value();

        VkShaderModule vertShaderModule = createShaderModule(vertex_shader_code, this->m_device->device).value();
        VkShaderModule fragShaderModule = createShaderModule(fragment_shader_code, this->m_device->device).value();

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
        auto bind = getBindingDescription<Utils::Vertex3D>();
        auto attribute = getAttributeDescriptions<Utils::Vertex3D>();
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

        /* ----- depth ----- */
        VkPipelineDepthStencilStateCreateInfo depth_stencil{};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.minDepthBounds = 0.0f; // Optional
        depth_stencil.maxDepthBounds = 1.0f; // Optional
        depth_stencil.stencilTestEnable = VK_FALSE;
        depth_stencil.front = {}; // Optional
        depth_stencil.back = {}; // Optional

        /* ----- create ----- */

        VkPipelineRenderingCreateInfo rendering_create_info{};
        rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_create_info.colorAttachmentCount = 1;
        rendering_create_info.pColorAttachmentFormats = &this->m_swapchain->swapchain_image_format;
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
        pipeline_info.pDepthStencilState = &depth_stencil;

        VkPipeline graphics_pipeline;
        if (vkCreateGraphicsPipelines(this->m_device->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create graphics pipeline!");
        }

        for (auto shader_module : shader_stages)
        {
            vkDestroyShaderModule(this->m_device->device, shader_module.module, nullptr);
        }

        std::unique_ptr<Pipeline3D> obj = std::make_unique<Pipeline3D>();
        obj->p_context = this;
        obj->self->graphics_pipeline = graphics_pipeline;
        obj->self->pipeline_layout = pipeline_layout;
        obj->self->descriptor_set_layouts = std::move(descriptor_set_layouts);
        return obj;
    }

    Pipeline3D::~Pipeline3D()
    {
        vkDestroyPipeline(this->p_context->m_device->device, this->self->graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(this->p_context->m_device->device, this->self->pipeline_layout, nullptr);
        for (const auto& layout : this->self->descriptor_set_layouts)
        {
            vkDestroyDescriptorSetLayout(this->p_context->m_device->device, layout, nullptr);
        }
    } 

}