#include "gpu.hpp"
#include "gpu/task/_task.hpp"
#include "gpu/binding/_binding.hpp"
#include "gpu/pipeline/_pipeline.hpp"
#include <vector>

namespace PWEngine::GPU 
{
    void RecordCommand::bindPipeline(IPipeline *pipeline)
    {
        switch (pipeline->type) 
        {
        case IPipeline::PipelineType::Render:
            vkCmdBindPipeline(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline);
        break;
        case IPipeline::PipelineType::Compute:
            vkCmdBindPipeline(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->self->pipeline);
        break;
        }
        
    }

    void RecordCommand::bindBinding(IPipeline* pipeline, std::vector<Binding*> bindings)
    {
        std::vector<VkDescriptorSet> descriptor_sets_list;
        for (const auto& binding : bindings)
        {
            descriptor_sets_list.push_back(binding->self->layout.at(this->p_task->flight_fences_index));
        }
        switch (pipeline->type) 
        {
        case IPipeline::PipelineType::Render:
            vkCmdBindDescriptorSets(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
        break;
        case IPipeline::PipelineType::Compute:
            vkCmdBindDescriptorSets(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
        break;
        }
        
    }


    void RecordCommand::pushConstant(IPipeline* pipeline, size_t offset, size_t data_size, void* data, ShaderType type)
    {
        VkShaderStageFlags stage;
        switch (type) 
        {
        case ShaderType::Vertex:
            stage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
        case ShaderType::Fragment:
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderType::Compute:
            stage = VK_SHADER_STAGE_COMPUTE_BIT;
        break;
        case ShaderType::VertexFragment:
            stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
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
        vkCmdPushConstants(this->p_task->self->command_buffers[this->p_task->flight_fences_index], pipeline->self->pipeline_layout, stage, offset, data_size, data);
    }

}