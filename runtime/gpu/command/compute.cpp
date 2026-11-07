#include "gpu.hpp"
#include "gpu/task/_task.hpp"
#include "gpu/binding/_binding.hpp"
#include <cstdint>
#include <vector>

namespace PWEngine::GPU 
{
    void RecordCommand::Dispatch(Vec3<uint32_t> group_size)
    {
        vkCmdDispatch(this->p_task->self->command_buffers[this->p_task->flight_fences_index], group_size.x, group_size.y, group_size.z);
    }
}