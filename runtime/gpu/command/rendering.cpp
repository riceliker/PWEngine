#include "gpu.hpp"
#include "gpu/task/_task.hpp"
#include "gpu/allocator/_allocator.hpp"
#include "gpu/binding/_binding.hpp"
#include "gpu/pipeline/_pipeline.hpp"
#include <cstdint>
#include <vector>

namespace PWEngine::GPU 
{
    void RenderingCommand::bindVertices(std::vector<GPUVertex*> buffers, std::vector<uint64_t> offset)
    {
        std::vector<VkBuffer> vertex_buffers; 
        for (const auto& buffer : buffers)
        {
            vertex_buffers.push_back(buffer->self->buffer);
        }
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(this->p_command->p_task->self->command_buffers[this->p_command->p_task->flight_fences_index], 0, vertex_buffers.size(), vertex_buffers.data(), offsets);
    }

    void RenderingCommand::bindIndex(GPUVertex* buffer, InputUintType size)
    {
        VkBuffer index_buffer = buffer->self->buffer;
        VkIndexType type;
        switch (size) 
        {
        case InputUintType::u8:
            type = VK_INDEX_TYPE_UINT8_KHR;
        break;
        case InputUintType::u16:
            type = VK_INDEX_TYPE_UINT16;
        break;
        case InputUintType::u32:
            type = VK_INDEX_TYPE_UINT32;
        break;
        }
        vkCmdBindIndexBuffer(this->p_command->p_task->self->command_buffers[this->p_command->p_task->flight_fences_index], index_buffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void RenderingCommand::bindBinding(std::vector<Binding*> bindings, Pipeline* pipeline)
    {
        std::vector<VkDescriptorSet> descriptor_sets_list;
        for (const auto& binding : bindings)
        {
            descriptor_sets_list.push_back(binding->self->layout.at(this->p_command->p_task->flight_fences_index));
        }
        vkCmdBindDescriptorSets(this->p_command->p_task->self->command_buffers[this->p_command->p_task->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->pipeline_layout, 0, descriptor_sets_list.size(), descriptor_sets_list.data(), 0, nullptr);
    }

    void RenderingCommand::draw(uint64_t vertex_count, uint64_t instance_count, uint64_t vertex_offset, uint64_t instance_offset)
    {
        vkCmdDraw(this->p_command->p_task->self->command_buffers[this->p_command->p_task->flight_fences_index], vertex_count, instance_count, vertex_offset, instance_offset);
    }
}
