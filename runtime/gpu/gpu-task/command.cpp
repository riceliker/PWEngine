#include "gpu.hpp"
#include "_task.hpp"
#include "gpu/instance/_instance.hpp"
#include "gpu/binding/_binding.hpp"
#include "gpu/pipeline/_pipeline.hpp"
#include <cstdint>
#include <memory>

namespace PWEngine::GPU
{
    void GPUTask::createCommandPool()
    {
        QueueFamilyIndices queue_family_indices = findQueueFamilies(this->p_instance->self->adapter);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

        if (vkCreateCommandPool(this->p_instance->self->device, &pool_info, nullptr, &this->self->command_pool) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::Vulkan, "failed to create command pool!");

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = this->self->command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(this->p_instance->self->device, &alloc_info, &this->self->command_buffer) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::Vulkan, "failed to allocate command buffers!");

    }

    void GPUTask::beginCommandInProcess()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(this->self->command_buffer, &beginInfo) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::Vulkan, "failed to begin recording command buffer!");
    }
    
    void GPUTask::endCommandInProcess()
    {
        if (vkEndCommandBuffer(this->self->command_buffer) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::Vulkan, "failed to record command buffer!");
    }

    void GPURecordCommand::bindPipeline(ComputePipeline *pipeline)
    {
        vkCmdBindPipeline(this->p_task->self->command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->self->pipeline);
    }

    void GPURecordCommand::bindBinding(ComputePipeline* pipeline, std::vector<Binding*> bindings)
    {
        std::vector<VkDescriptorSet> descriptor_sets_list;
        for (const auto& binding : bindings)
        {
            descriptor_sets_list.push_back(binding->self->layout.at(0));
        }
        switch (pipeline->type) 
        {
        case IPipeline::PipelineType::Render:
            vkCmdBindDescriptorSets(this->p_task->self->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
        break;
        case IPipeline::PipelineType::Compute:
            vkCmdBindDescriptorSets(this->p_task->self->command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
        break;
        }
        
    }

    void GPURecordCommand::pushConstant(ComputePipeline* pipeline, size_t offset, size_t data_size, void* data, ShaderType type)
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
        vkCmdPushConstants(this->p_task->self->command_buffer, pipeline->self->pipeline_layout, stage, offset, data_size, data);
    }

    void GPURecordCommand::Dispatch(Vec3<uint32_t> group_size)
    {
        vkCmdDispatch(this->p_task->self->command_buffer, group_size.x, group_size.y, group_size.z);
    }
}