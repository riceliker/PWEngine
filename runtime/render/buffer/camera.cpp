#include "render.hpp"
#include "impl.hpp"
#include "../context/impl.hpp"
#include "buffer.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace PWEngine::Render 
{
    Camera::Camera() : self(std::make_unique<Impl>()){}
    Camera::~Camera()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            vkDestroyBuffer(this->p_context->m_device->device, this->self->uniform_buffers[i], nullptr);
            vkFreeMemory(this->p_context->m_device->device, this->self->uniform_buffers_memory[i], nullptr);
        }
    }
    
    std::unique_ptr<Camera> RenderContext::creatCamera(size_t descriptor_set_layout_index)
    {
        auto obj = std::make_unique<Camera>();
        obj->p_context = this;
        obj->setUniform(this, descriptor_set_layout_index);
        return obj;
    }

    void Camera::setUniform(RenderContext* super, size_t descriptor_set_layout_index)
    {
        VkDeviceSize buffer_size = sizeof(Camera::CameraData);

        this->self->uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        this->self->uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        this->self->uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createDirectBuffer(super, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, this->self->uniform_buffers[i], this->self->uniform_buffers_memory[i]);
            vkMapMemory(super->m_device->device, this->self->uniform_buffers_memory[i], 0, buffer_size, 0, &this->self->uniform_buffers_mapped[i]);
        }

        VkDescriptorSetLayout descriptor_set_layout = super->self->descriptor_set_layouts[descriptor_set_layout_index];
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptor_set_layout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = super->self->descriptor_pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptor_sets;

        descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(super->m_device->device, &allocInfo, descriptor_sets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        this->self->descriptor_sets = std::move(descriptor_sets);
    }

    void Camera::update(uint32_t current_frame)
    {
        this->data.view = Utils::look(this->position, this->position + this->look_vector, Utils::Vec3<float>(0.0f, 0.0f, 1.0f));
        this->data.project = Utils::perspective(Utils::deg2rad(view_degree), this->p_context->m_swapchain->swapchain_extent.width / (float) this->p_context->m_swapchain->swapchain_extent.height, this->view_depth.x , this->view_depth.y);
        this->data.project.rc(1, 1) *= -1;

        VkDescriptorBufferInfo camera_info{};
        camera_info.buffer = this->self->uniform_buffers[current_frame];
        camera_info.offset = 0;
        camera_info.range = sizeof(Camera::CameraData); /* size 128: 2 * mat4 */

        std::array<VkWriteDescriptorSet, 1> descriptor_writes{};
        descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_writes[0].dstSet = this->self->descriptor_sets[current_frame];
        descriptor_writes[0].dstBinding = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo = &camera_info;
        
        vkUpdateDescriptorSets(this->p_context->m_device->device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
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