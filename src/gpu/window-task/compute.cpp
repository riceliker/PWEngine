#include "gpu.hpp"
#include "_task.hpp"
#include <cstdint>
#include <vector>

namespace PWEngine::GPU 
{
    void WindowRecordCommand::Dispatch(Vec3<uint32_t> group_size)
    {
        vkCmdDispatch(this->p_task->self->command_buffers[this->p_task->flight_fences_index], group_size.x, group_size.y, group_size.z);
    }
}