#include "gpu.hpp"
#include "gpu/task/_task.hpp"
#include "gpu/binding/_binding.hpp"
#include "gpu/pipeline/_pipeline.hpp"
#include <vector>

namespace PWEngine::GPU 
{
    void RecordCommand::bindPipeline(RenderPipeline *pipeline)
    {
        vkCmdBindPipeline(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
    }

    void RecordCommand::bindPipeline(ComputePipeline *pipeline)
    {
        vkCmdBindPipeline(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->self->compute_pipeline);
    }

    void RecordCommand::bindBinding(RenderPipeline* pipeline, std::vector<Binding*> bindings)
    {
        std::vector<VkDescriptorSet> descriptor_sets_list;
        for (const auto& binding : bindings)
        {
            descriptor_sets_list.push_back(binding->self->layout.at(this->p_task->flight_fences_index));
        }
        vkCmdBindDescriptorSets(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
    }

    void RecordCommand::bindBinding(ComputePipeline* pipeline, std::vector<Binding*> bindings)
    {
        std::vector<VkDescriptorSet> descriptor_sets_list;
        for (const auto& binding : bindings)
        {
            descriptor_sets_list.push_back(binding->self->layout.at(this->p_task->flight_fences_index));
        }
        vkCmdBindDescriptorSets(this->p_task->self->command_buffers[this->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
    }

    void RecordCommand::pushConstant(RenderPipeline* pipeline, size_t offset, size_t data_size, void* data, ShaderType type)
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

    void RecordCommand::pushConstant(ComputePipeline* pipeline, size_t offset, size_t data_size, void* data, ShaderType type)
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