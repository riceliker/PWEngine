#include "render.hpp"
#include "render/_render.hpp"
#include "render/_utils.hpp"
#include <cstddef>
#include <memory>
#include <utility>

namespace PWEngine::Render 
{
    Mesh3D::Mesh3D():m_vertex(std::make_unique<Vertex3D>()), m_descriptor_set(std::make_unique<DescriptorSet>())
    {
    }

    std::unique_ptr<Mesh3D> Pipeline3D::createMesh3D(Model3D* model)
    {
        auto obj = std::make_unique<Mesh3D>();
        obj->p_pipeline = this;
        obj->indices_size = model->indices.size();
        obj->setVertices(this->p_context, model->vertices);
        obj->setIndices(this->p_context, model->indices);
        obj->m_descriptor_set->descriptor_sets = std::move(createDescriptorSet(this, this->self->descriptor_set_layouts[0]));
        return obj;
    }

    void Mesh3D::setVertices(RenderContext* super, std::vector<PWEngine::Vertex3D>& vertices)
    {
        size_t size = sizeof(vertices[0]) * vertices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createStagingBuffer(super, size, staging_buffer, staging_buffer_memory);

        /* data */
        void* data;
        vkMapMemory(super->m_device->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, vertices.data(), (size_t) size);
        vkUnmapMemory(super->m_device->device, staging_buffer_memory);

        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        createRealBuffer(super, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer, vertex_buffer_memory);

        copyBufferCommand(super, staging_buffer, vertex_buffer, size);

        vkDestroyBuffer(super->m_device->device, staging_buffer, nullptr);
        vkFreeMemory(super->m_device->device, staging_buffer_memory, nullptr);

        this->m_vertex->vertex_buffer = vertex_buffer;
        this->m_vertex->vertex_buffer_memory = vertex_buffer_memory;
    }

    void Mesh3D::setIndices(RenderContext* super, std::vector<uint32_t>& indices)
    {
        size_t size = sizeof(indices[0]) * indices.size();

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createStagingBuffer(super, size, staging_buffer, staging_buffer_memory);

        /* data */
        void* data;
        vkMapMemory(super->m_device->device, staging_buffer_memory, 0, size, 0, &data);
        memcpy(data, indices.data(), (size_t) size);
        vkUnmapMemory(super->m_device->device, staging_buffer_memory);

        VkBuffer index_buffer;
        VkDeviceMemory index_buffer_memory;
        createRealBuffer(super, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer, index_buffer_memory);

        copyBufferCommand(super, staging_buffer, index_buffer, size);

        vkDestroyBuffer(super->m_device->device, staging_buffer, nullptr);
        vkFreeMemory(super->m_device->device, staging_buffer_memory, nullptr);
        
        this->m_vertex->indices_buffer = index_buffer;
        this->m_vertex->indices_buffer_memory = index_buffer_memory;
    }

    void Mesh3D::bindTransform3D(std::shared_ptr<Transform3D> transform)
    {
        this->transform = transform;
        /* DescriptorSet */
        for (size_t slot = 0; slot < MAX_FRAMES_IN_FLIGHT; slot++)
        {
            VkDescriptorBufferInfo ssbo_info{};
            ssbo_info.buffer = transform->m_uniform->self->uniform_buffers[slot];
            ssbo_info.offset = 0;
            ssbo_info.range = VK_WHOLE_SIZE;

            std::vector<VkWriteDescriptorSet> descriptor_writes{};
            descriptor_writes.resize(1);
            descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[0].dstSet = this->m_descriptor_set->descriptor_sets.at(slot);
            descriptor_writes[0].dstBinding = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo = &ssbo_info;

            vkUpdateDescriptorSets(this->p_pipeline->p_context->m_device->device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
        }
    }

    Mesh3D::~Mesh3D()
    {
        /* vertex */
        vkDestroyBuffer(this->p_pipeline->p_context->m_device->device, this->m_vertex->vertex_buffer, nullptr);
        vkFreeMemory(this->p_pipeline->p_context->m_device->device, this->m_vertex->vertex_buffer_memory, nullptr);
        vkDestroyBuffer(this->p_pipeline->p_context->m_device->device, this->m_vertex->indices_buffer, nullptr);
        vkFreeMemory(this->p_pipeline->p_context->m_device->device, this->m_vertex->indices_buffer_memory, nullptr);
    }
}