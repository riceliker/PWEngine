#include "gpu.hpp"
#include "gpu/task/_task.hpp"
#include "gpu/pipeline/_pipeline.hpp"

namespace PWEngine::GPU 
{
    void RecordCommand::bindPipeline(IPipeline *pipeline, PipelineType type)
    {
        switch (type) 
        {
        case PipelineType::Render:
            vkCmdBindPipeline(this->p_task->self->command_buffers[this->p_task->self->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
        break;
        case PipelineType::Compute:
            vkCmdBindPipeline(this->self->command_buffers[this->flight_fences_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->self->graphics_pipeline);
        break;
        }
    }
}