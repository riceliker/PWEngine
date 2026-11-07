#include "gpu.hpp"
#include "gpu/instance/_instance.hpp"
#include "gpu/binding/_binding.hpp"
#include "_pipeline.hpp"

#include "utils.hpp"
#include <memory>
#include <optional>
#include <vector>
namespace PWEngine::GPU
{
    ComputePipeline::ComputePipeline(GPUInstance* instance, ComputePipelineInfo info) : IPipeline()
    {
        this->p_instance= instance;
        this->type = IPipeline::PipelineType::Compute;

        VkComputePipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;

        /* --- Shader Module --- */
        std::optional<VkShaderModule> compute_shader_module_maybe = createShaderModule(info.compute_shader_code, this->p_instance->self->device);
        if (!compute_shader_module_maybe.has_value()) 
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "Can not read Vertex Shader.");

        VkPipelineShaderStageCreateInfo compute_shader_stage_info{};
        compute_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compute_shader_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        compute_shader_stage_info.module = compute_shader_module_maybe.value();
        compute_shader_stage_info.pName = "main";

        pipeline_info.stage = compute_shader_stage_info;
        
        /* --- Binding Layout --- */
        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
        if (info.binding_layout.size() !=0)
        {
            for(const auto& binding_layout : info.binding_layout)
            {
                descriptor_set_layouts.push_back(binding_layout->self->descriptor_set_layout);
            }
        }
        
        std::vector<VkPushConstantRange> push_constant_ranges;
        if (info.push_constant_layout.size() != 0)
        {
            for (const auto& range : info.push_constant_layout)
            {
                VkShaderStageFlags stage;
                switch (range.type) 
                {
                case ShaderType::Vertex:
                    sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "You can not registry graphic only binding in compute pipeline.");
                break;
                case ShaderType::Fragment:
                    sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "You can not registry graphic only binding in compute pipeline.");
                break;
                case ShaderType::Compute:
                    stage = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
                case ShaderType::VertexFragment:
                    sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "You can not registry graphic only binding in compute pipeline.");
                break;
                case ShaderType::VertexCompute:
                    stage = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT;
                break;
                case ShaderType::FragmentCompute:
                    stage = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
                case ShaderType::VertexFragmentCompute:
                    stage = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
                }
                VkPushConstantRange push_range{};

                push_range.stageFlags = stage;
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
        pipeline_info.layout = pipeline_layout;

        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        VkPipeline compute_pipeline;
        if (vkCreateComputePipelines(this->p_instance->self->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &compute_pipeline) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create compute pipeline!");

        vkDestroyShaderModule(this->p_instance->self->device, compute_shader_module_maybe.value(), nullptr);

        self->pipeline = compute_pipeline;
    }

    ComputePipeline::~ComputePipeline()
    {
        vkDestroyPipeline(this->p_instance->self->device, this->self->pipeline, nullptr);
        vkDestroyPipelineLayout(this->p_instance->self->device, this->self->pipeline_layout, nullptr);
    }
}