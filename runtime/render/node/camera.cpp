#include "render.hpp"
#include "impl.hpp"
#include "buffer.hpp"
#include "../node/impl.hpp"
#include "../context/impl.hpp"
#include "buffer.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    Camera::Camera() : m_uniform(std::make_unique<Uniform>()), m_descriptor_set(std::make_unique<DescriptorSet>())
    {

    }

    Camera::~Camera()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            vkDestroyBuffer(this->p_context->m_device->device, this->m_uniform->uniform_buffers[i], nullptr);
            vkFreeMemory(this->p_context->m_device->device, this->m_uniform->uniform_buffers_memory[i], nullptr);
        }
    }
    
    std::unique_ptr<Camera> Pipeline3D::creatCamera()
    {
        auto obj = std::make_unique<Camera>();
        obj->p_context = this->p_context;
        obj->p_pipeline = this;

        VkDeviceSize buffer_size = sizeof(Camera::CameraData);

        obj->m_uniform->uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        obj->m_uniform->uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        obj->m_uniform->uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createDirectBuffer(this->p_context, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, obj->m_uniform->uniform_buffers[i], obj->m_uniform->uniform_buffers_memory[i]);
            vkMapMemory(this->p_context->m_device->device, obj->m_uniform->uniform_buffers_memory[i], 0, buffer_size, 0, &obj->m_uniform->uniform_buffers_mapped[i]);
        }

        obj->m_descriptor_set->descriptor_sets = createDescriptorSet(this, this->self->descriptor_set_layouts[2]);

        return obj;
    }

    void Camera::update(uint32_t swapchain_loop_frame_index)
    {
        this->data.view = Utils::look(this->position, this->position + this->look_vector, Utils::Vec3<float>(0.0f, 0.0f, 1.0f));
        this->data.project = Utils::perspective(Utils::deg2rad(view_degree), this->p_context->m_swapchain->swapchain_extent.width / (float) this->p_context->m_swapchain->swapchain_extent.height, this->view_depth.x , this->view_depth.y);
        this->data.project.rc(1, 1) *= -1;

        VkDescriptorBufferInfo camera_info{};
        camera_info.buffer = this->m_uniform->uniform_buffers[swapchain_loop_frame_index];
        camera_info.offset = 0;
        camera_info.range = sizeof(Camera::CameraData); /* size 128: 2 * mat4 */

        std::array<VkWriteDescriptorSet, 1> descriptor_writes{};
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = this->m_descriptor_set->descriptor_sets.at(swapchain_loop_frame_index);
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &camera_info;
        
        vkUpdateDescriptorSets(this->p_context->m_device->device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);        

        memcpy(this->m_uniform->uniform_buffers_mapped[this->p_context->image_index], &this->data, sizeof(Camera::CameraData));
    }

    void Camera::calculateLookVector(Utils::Vec2<float> look_degree)
    {
        this->look_vector = PWEngine::Utils::Vec3<float>(cos(PWEngine::Utils::deg2rad(look_degree.y)), sin(PWEngine::Utils::deg2rad(look_degree.y)), sin(PWEngine::Utils::deg2rad(look_degree.x)));
    }

    void Camera::cameraMoveFromLook(Utils::Vec2<float> step)
    {
        this->position.x += step.x * this->look_vector.x;
        this->position.y += step.x * this->look_vector.y;
        this->position.z += step.x * this->look_vector.z;

        this->position.x += step.y * this->look_vector.y;
        this->position.y -= step.y * this->look_vector.x;
        this->position.z -= step.y * this->look_vector.z;
    }
}