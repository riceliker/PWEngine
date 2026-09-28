#include "render.hpp"
#include "render/_render.hpp"
#include <memory>


namespace PWEngine::Render 
{
    Node3D::Node3D(Pipeline3D* pipeline) : m_descriptor_set(std::make_unique<DescriptorSet>())
    {
        this->p_pipeline = pipeline;
    }   
        
    void Node3D::bindMesh3D(std::shared_ptr<Mesh3D> mesh)
    {
        this->m_mesh = mesh;
    }

    void Node3D::bindMaterial(std::shared_ptr<Material> material)
    {
        this->m_material = material;
    }

    void Node3D::bindTransform3D(std::shared_ptr<Transform3D> transform3D)
    {
        this->m_transform = transform3D;
    }
    
    void Node3D::updateDescriptor()
    {
        for (size_t slot = 0; slot < MAX_FRAMES_IN_FLIGHT; slot++)
        {
            VkDescriptorBufferInfo ssbo_info{};
            ssbo_info.buffer = this->m_transform.value()->m_uniform->self->uniform_buffers[slot];
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

    Node3D::~Node3D()
    {

    }
}