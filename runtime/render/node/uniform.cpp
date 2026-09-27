#include "render.hpp"
#include "render/_render.hpp"
#include "render/_utils.hpp"
#include <cstddef>
#include <memory>

namespace PWEngine::Render 
{
    Uniform::Uniform(RenderContext* p_context): self(std::make_unique<Impl>())
    {

    }

    std::shared_ptr<Uniform> RenderContext::createUniform(size_t size)
    {
        std::shared_ptr<Uniform> obj = std::make_shared<Uniform>(this);
        obj->p_context = this;
        obj->self->uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        obj->self->uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        obj->self->uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createDirectBuffer(this, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, obj->self->uniform_buffers[i], obj->self->uniform_buffers_memory[i]);
            vkMapMemory(this->m_device->device, obj->self->uniform_buffers_memory[i], 0, size, 0, &obj->self->uniform_buffers_mapped[i]);
        }
        return obj;
    }

    void Uniform::updateData(Command* command, void* data, size_t size)
    {
        this->data = data;
        memcpy(this->self->uniform_buffers_mapped[command->swapchain_image_index], data, size);  
    }

    Uniform::~Uniform()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            vkDestroyBuffer(this->p_context->m_device->device, this->self->uniform_buffers[i], nullptr);
            vkFreeMemory(this->p_context->m_device->device, this->self->uniform_buffers_memory[i], nullptr);
        }
    }
}