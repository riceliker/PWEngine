#include "gpu.hpp"
#include "gpu/task/_task.hpp"
#include "gpu/allocator/_allocator.hpp"
#include <cstdint>
#include <vector>

namespace PWEngine::GPU 
{
    void RenderingCommand::bindVertices(std::vector<GPUBuffer*> buffers, std::vector<uint64_t> offset)
    {
        std::vector<VkBuffer> vertex_buffers; 
        for (const auto& buffer : buffers)
        {
            vertex_buffers.push_back(buffer->self->buffer);
        }
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(this->p_command->p_task->self->command_buffers[this->p_command->p_task->flight_fences_index], 0, vertex_buffers.size(), vertex_buffers.data(), offsets);
    }

    void RenderingCommand::bindIndex(GPUBuffer* buffer, InputUintType size)
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
    void RenderingCommand::draw(uint64_t vertex_count, uint64_t instance_count, uint64_t vertex_offset, uint64_t instance_offset)
    {
        vkCmdDraw(this->p_command->p_task->self->command_buffers[this->p_command->p_task->flight_fences_index], vertex_count, instance_count, vertex_offset, instance_offset);
    }
}
