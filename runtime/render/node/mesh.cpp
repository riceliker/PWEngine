#include "render.hpp"
#include "utils.hpp"
#include "impl.hpp"
#include "../context/impl.hpp"
#include "buffer.hpp"
#include <cstddef>
#include <memory>
#include <utility>

namespace PWEngine::Render 
{
    Mesh3D::Mesh3D():m_vertex(std::make_unique<Vertex3D>()), m_descriptor_set(std::make_unique<DescriptorSet>()), m_uniform(std::make_unique<Uniform>())
    {
        this->position = {0, 0, 0};
        this->rotation = {0, 0, 0, 1};
        this->scale = {1, 1, 1};
    }

    std::unique_ptr<Mesh3D> Pipeline3D::createMesh3D(Utils::Model3D* model)
    {
        auto obj = std::make_unique<Mesh3D>();
        obj->p_pipeline = this;
        obj->indices_size = model->indices.size();
        obj->setVertices(this->p_context, model->vertices);
        obj->setIndices(this->p_context, model->indices);
        obj->setUniform(this);
        obj->m_descriptor_set->descriptor_sets = std::move(createDescriptorSet(this, this->self->descriptor_set_layouts[0]));
        return obj;
    }

    void Mesh3D::setUniform(Pipeline3D* super)
    {
        VkDeviceSize bufferSize = sizeof(Mesh3D::UniformData);

        this->m_uniform->uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        this->m_uniform->uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        this->m_uniform->uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createDirectBuffer(super->p_context, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, this->m_uniform->uniform_buffers[i], this->m_uniform->uniform_buffers_memory[i]);
            vkMapMemory(super->p_context->m_device->device, this->m_uniform->uniform_buffers_memory[i], 0, bufferSize, 0, &this->m_uniform->uniform_buffers_mapped[i]);
        }
    }

    void Mesh3D::calculateNodeMatrix()
    {
        this->data.transform = Utils::transform(this->position, this->scale, this->rotation);
    }

    void Mesh3D::updateDescriptor()
    {
        /* DescriptorSet */
        for (size_t slot = 0; slot < MAX_FRAMES_IN_FLIGHT; slot++)
        {
            VkDescriptorBufferInfo model_info{};
            model_info.buffer = this->m_uniform->uniform_buffers[slot];
            model_info.offset = 0;
            model_info.range = sizeof(Mesh3D::UniformData); /* size 64: 1 * mat4 */

            std::vector<VkWriteDescriptorSet> descriptor_writes{};
            descriptor_writes.resize(1);
            descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[0].dstSet = this->m_descriptor_set->descriptor_sets.at(slot);
            descriptor_writes[0].dstBinding = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo = &model_info;

            vkUpdateDescriptorSets(this->p_pipeline->p_context->m_device->device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
        }
    }
    void Mesh3D::updateData(Command* command)
    {
        memcpy(this->m_uniform->uniform_buffers_mapped[command->swapchain_image_index], &this->data, sizeof(Mesh3D::UniformData));  
    }
    Mesh3D::~Mesh3D()
    {
        /* vertex */
        vkDestroyBuffer(this->p_pipeline->p_context->m_device->device, this->m_vertex->vertex_buffer, nullptr);
        vkFreeMemory(this->p_pipeline->p_context->m_device->device, this->m_vertex->vertex_buffer_memory, nullptr);
        vkDestroyBuffer(this->p_pipeline->p_context->m_device->device, this->m_vertex->indices_buffer, nullptr);
        vkFreeMemory(this->p_pipeline->p_context->m_device->device, this->m_vertex->indices_buffer_memory, nullptr); 

        /* uniform */
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            vkDestroyBuffer(this->p_pipeline->p_context->m_device->device, this->m_uniform->uniform_buffers[i], nullptr);
            vkFreeMemory(this->p_pipeline->p_context->m_device->device, this->m_uniform->uniform_buffers_memory[i], nullptr);
        }
    }
}