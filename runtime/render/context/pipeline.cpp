#include "render.hpp"
#include "impl.hpp"
#include "../buffer/impl.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstddef>
#include <fstream>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    Pipeline::Pipeline() : self(std::make_unique<Impl>())
    {

    }
    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
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

    static inline std::vector<VkPipelineShaderStageCreateInfo> createShader(RenderContext* context)
    {
        /* shader */
        auto vertShaderCode = readFile("./shaders/vert.spv");
        auto fragShaderCode = readFile("./shaders/frag.spv");

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode, context->m_device->device).value();
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode, context->m_device->device).value();

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {vertShaderStageInfo, fragShaderStageInfo};
        return shaderStages;
    }

    static inline VkPipelineVertexInputStateCreateInfo createVertexFormat(VkVertexInputBindingDescription& bind, std::array<VkVertexInputAttributeDescription, 3>& attribute)
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute.size());
        vertexInputInfo.pVertexBindingDescriptions = &bind;
        vertexInputInfo.pVertexAttributeDescriptions = attribute.data();
        return vertexInputInfo;
    }

    std::unique_ptr<Pipeline> RenderContext::createPipeline(size_t descriptor_set_layout_index)
    {   
        VkDescriptorSetLayout descriptor_set_layout = this->self->descriptor_set_layouts.at(descriptor_set_layout_index);
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;

        auto shaderStages = createShader(this);
        auto bind = getBindingDescription<Utils::Vertex3D>();
        auto attribute = getAttributeDescriptions<Utils::Vertex3D>();
        auto vertexInputInfo = createVertexFormat(bind, attribute);
        
        /* InputAssembly: default */
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        /* Dynamic */
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; /* default */
        rasterizer.rasterizerDiscardEnable = VK_FALSE; /* default: It must be render */
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL; /* default */
        rasterizer.lineWidth = 1.0f; /* default */
        rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT; /* default: backend face must be ignored */
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; /* default */
        rasterizer.depthBiasEnable = VK_FALSE; /* dynamic: see setDepthBias() */

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptor_set_layout;

        if (vkCreatePipelineLayout(this->m_device->device, &pipelineLayoutInfo, nullptr, &pipeline_layout) != VK_SUCCESS) {
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create pipeline layout!");
        }

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


        VkPipelineRenderingCreateInfo rendering_create_info{};
        rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_create_info.colorAttachmentCount = 1;
        rendering_create_info.pColorAttachmentFormats = &this->m_swapchain->swapchain_image_format;
        rendering_create_info.depthAttachmentFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        rendering_create_info.stencilAttachmentFormat = VK_FORMAT_D24_UNORM_S8_UINT;
        

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext = &rendering_create_info;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shaderStages.data();
        pipeline_info.pVertexInputState = &vertexInputInfo;
        pipeline_info.pInputAssemblyState = &inputAssembly;
        pipeline_info.pViewportState = &viewportState;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pColorBlendState = &colorBlending;
        pipeline_info.pDynamicState = &dynamicState;
        pipeline_info.layout = pipeline_layout;
        pipeline_info.renderPass = VK_NULL_HANDLE;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_info.pDepthStencilState = &depth_stencil;


        if (vkCreateGraphicsPipelines(this->m_device->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create graphics pipeline!");
        }

        for (auto shader_module : shaderStages)
        {
            vkDestroyShaderModule(this->m_device->device, shader_module.module, nullptr);
        }

        std::unique_ptr<Pipeline> obj = std::make_unique<Pipeline>();
        obj->p_context = this;
        obj->self->graphics_pipeline = graphics_pipeline;
        obj->self->pipeline_layout = pipeline_layout;
        return obj;
    }

    Pipeline::~Pipeline()
    {
        vkDestroyPipeline(this->p_context->m_device->device, this->self->graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(this->p_context->m_device->device, this->self->pipeline_layout, nullptr);
    }
}