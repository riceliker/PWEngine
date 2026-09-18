#include "render.hpp"
#include "utils.hpp"
#include "impl.hpp"
#include "../command/impl.hpp"
#include "../context/impl.hpp"
#include "buffer.hpp"
#include <cstddef>
#include <memory>

namespace PWEngine::Render 
{
    Mesh3D::Mesh3D():m_vertex(std::make_unique<Vertex3D>()), m_texture(std::make_unique<Texture2D>()), m_uniform(std::make_unique<Uniform>()){}

    std::unique_ptr<Mesh3D> Pipeline3D::createMesh3D(Utils::Model3D* model, Utils::ImageRGBA8* image)
    {
        auto obj = std::make_unique<Mesh3D>();
        obj->p_context = this->p_context;
        obj->p_pipeline = this;
        obj->indices_size = model->indices.size();
        obj->setVertices(this->p_context, model->vertices);
        obj->setIndices(this->p_context, model->indices);
        obj->setTexture2D(this->p_context, *image);
        obj->setTextureView(this->p_context);
        obj->setTextureSampler(this->p_context);
        obj->setUniform(this);
        return obj;
    }

    void Mesh3D::setUniform(Pipeline3D* super)
    {
        VkDeviceSize bufferSize = sizeof(Mesh3D::UBOData);

        this->m_uniform->uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        this->m_uniform->uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        this->m_uniform->uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createDirectBuffer(super->p_context, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, this->m_uniform->uniform_buffers[i], this->m_uniform->uniform_buffers_memory[i]);
            vkMapMemory(super->p_context->m_device->device, this->m_uniform->uniform_buffers_memory[i], 0, bufferSize, 0, &this->m_uniform->uniform_buffers_mapped[i]);
        }
    }

    void Mesh3D::update(uint32_t current_frame)
    {
        VkDescriptorBufferInfo model_info{};
        model_info.buffer = this->m_uniform->uniform_buffers[current_frame];
        model_info.offset = 0;
        model_info.range = sizeof(Mesh3D::UBOData); /* size 64: 1 * mat4 */

        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = this->m_texture->texture_image_view;
        image_info.sampler = this->m_texture->texture_sampler;

        std::array<VkWriteDescriptorSet, 2> descriptor_writes{};
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = this->p_pipeline->self->descriptor_sets.at(0).at(current_frame);
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &model_info;

        descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[1].dstSet = this->p_pipeline->self->descriptor_sets.at(0).at(current_frame);
        descriptor_writes[1].dstBinding = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_writes[1].descriptorCount = 1;
        descriptor_writes[1].pImageInfo = &image_info;

        vkUpdateDescriptorSets(this->p_context->m_device->device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
       
    }

    void Mesh3D::setData()
    {
        memcpy(this->m_uniform->uniform_buffers_mapped[this->p_context->image_index], &this->data, sizeof(Mesh3D::UBOData));
    }

    void Mesh3D::bind(FrameCommandRendering* rendering)
    {
        VkBuffer vertex_buffers[] = {this->m_vertex->vertex_buffer};
        VkBuffer index_buffer = this->m_vertex->indices_buffer;
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(rendering->self->command_buffer, 0, 1, vertex_buffers, offsets);
        vkCmdBindIndexBuffer(rendering->self->command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(rendering->self->command_buffer, this->indices_size, 1, 0, 0, 0);
    }

    Mesh3D::~Mesh3D()
    {
        /* texture */
        vkDestroySampler(this->p_context->m_device->device, this->m_texture->texture_sampler, nullptr);
        vkDestroyImageView(this->p_context->m_device->device, this->m_texture->texture_image_view, nullptr);
        vkDestroyImage(this->p_context->m_device->device, this->m_texture->texture_image, nullptr);
        vkFreeMemory(this->p_context->m_device->device, this->m_texture->texture_image_memory, nullptr);

        /* vertex */
        vkDestroyBuffer(this->p_context->m_device->device, this->m_vertex->vertex_buffer, nullptr);
        vkFreeMemory(this->p_context->m_device->device, this->m_vertex->vertex_buffer_memory, nullptr);
        vkDestroyBuffer(this->p_context->m_device->device, this->m_vertex->indices_buffer, nullptr);
        vkFreeMemory(this->p_context->m_device->device, this->m_vertex->indices_buffer_memory, nullptr); 

        /* uniform */
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            vkDestroyBuffer(this->p_context->m_device->device, this->m_uniform->uniform_buffers[i], nullptr);
            vkFreeMemory(this->p_context->m_device->device, this->m_uniform->uniform_buffers_memory[i], nullptr);
        }
    }
}