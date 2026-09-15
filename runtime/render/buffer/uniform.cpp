#include "render.hpp"
#include "../context/impl.hpp"
#include "../buffer/buffer.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    UBO::UBO() : self(std::make_unique<Impl>())
    {
        
    }
    UBO::~UBO()
    {

    }
    std::unique_ptr<UBO> RenderContext::createUBO()
    {
        VkDeviceSize bufferSize = sizeof(UBO::UBOData);

        std::unique_ptr<UBO> obj = std::make_unique<UBO>();
        obj->p_context = this;
        obj->self->uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        obj->self->uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        obj->self->uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createDirectBuffer(this, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, obj->self->uniform_buffers[i], obj->self->uniform_buffers_memory[i]);
            vkMapMemory(this->m_device->device, obj->self->uniform_buffers_memory[i], 0, bufferSize, 0, &obj->self->uniform_buffers_mapped[i]);
        }
        
        return obj;
    }
    
}