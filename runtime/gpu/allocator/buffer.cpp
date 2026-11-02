#include "_allocator.hpp"
#include "gpu.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
namespace PWEngine::GPU 
{
    GPUBuffer::GPUBuffer(GPUInstance* p_instance, GPUBufferDataType type, size_t byte_size, void* data) : self(std::make_unique<Impl>())
    {
        this->p_instance = p_instance;
        createGPUBuffer(byte_size, data, type);
    }

    void GPUBuffer::createGPUBuffer(size_t size, void* element, GPUBufferDataType type)
    {
        CreateBufferInfo staging_info{};
        staging_info.size = size;
        staging_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        staging_info.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
 
        auto [staging_buffer, staging_buffer_memory] = createBuffer(p_instance, staging_info).value();

        /* data */
        void* data;
        vkMapMemory(p_instance->self->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, element, size);
        vkUnmapMemory(p_instance->self->device, staging_buffer_memory);

        CreateBufferInfo vertex_info{};
        vertex_info.size = size;
        switch (type) 
        {
        case GPUBufferDataType::vertex:
            vertex_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        break;
        case GPUBufferDataType::index:
            vertex_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        break;
        }
        vertex_info.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        
        auto [vertex_buffer, vertex_buffer_memory] = createBuffer(p_instance, vertex_info).value();

        copyBufferCommand(p_instance, staging_buffer, vertex_buffer, size);

        vkDestroyBuffer(p_instance->self->device, staging_buffer, nullptr);
        vkFreeMemory(p_instance->self->device, staging_buffer_memory, nullptr);

        this->self->buffer = vertex_buffer;
        this->self->memory = vertex_buffer_memory;
    } 
    
    GPUBuffer::~GPUBuffer()
    {
        vkDestroyBuffer(this->p_instance->self->device, this->self->buffer, nullptr);
        vkFreeMemory(this->p_instance->self->device, this->self->memory, nullptr);
    }

    GPUBufferView::GPUBufferView(GPUBuffer* buffer, BufferFormat format, uint32_t offset, uint32_t range) : self(std::make_unique<Impl>())
    {
        this->buffer = buffer;
        VkBufferView buffer_view;
        VkBufferViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
        view_info.buffer = this->buffer->self->buffer;
        switch (format) 
        {
        case BufferFormat::i32:
            view_info.format = VK_FORMAT_R32_SINT;
        break;
        case BufferFormat::u32:
            view_info.format = VK_FORMAT_R32_UINT;
        break;
        case BufferFormat::f32:
            view_info.format = VK_FORMAT_R32_SFLOAT;
        break;
        case BufferFormat::vec2:
            view_info.format = VK_FORMAT_R32G32_SFLOAT;
        break;
        case BufferFormat::vec3:
            view_info.format = VK_FORMAT_R32G32B32_SFLOAT;
        break;
        case BufferFormat::vec4:
            view_info.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        break;
        case BufferFormat::color:
            view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        break;
        }
        view_info.flags = 0;
        view_info.offset = offset;
        view_info.range = range;
        if (vkCreateBufferView(this->buffer->p_instance->self->device, &view_info, nullptr, &buffer_view) != VK_SUCCESS)
            sendLog(this->buffer->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create texture image view!");

        this->self->view = buffer_view;
    }

    GPUBufferView::~GPUBufferView()
    {
        vkDestroyBufferView(this->buffer->p_instance->self->device, this->self->view, nullptr);
    }

    size_t GPUBuffer::createView(BufferFormat format, uint32_t offset, uint32_t range)
    {
        auto view = std::make_unique<GPUBufferView>(this, format, offset, range);
        this->view_map.insert({index_count, std::move(view)});
        return index_count++;
    }

    std::optional<GPUBufferView*> GPUBuffer::getView(size_t index)
    {
        auto result = this->view_map.find(index);
        if (result == this->view_map.end())
            return std::nullopt;
        return result->second.get();
    }
}