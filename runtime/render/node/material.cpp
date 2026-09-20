#include "render.hpp"
#include "buffer.hpp"
#include "impl.hpp"
#include "../context/impl.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    Material::Material() : m_descriptor_set(std::make_unique<DescriptorSet>()), m_uniform(std::make_unique<Uniform>())
    {

    }

    std::unique_ptr<Material> Pipeline3D::createMaterial()
    {
        auto obj = std::make_unique<Material>();
        obj->p_pipeline = this;
        obj->m_descriptor_set->descriptor_sets = createDescriptorSet(this, this->self->descriptor_set_layouts.at(1));
        return obj;
    }

    void Material::bindBasicTexture(std::shared_ptr<Texture2D> texture)
    {   
        this->basic_texture = texture;
    }

    void Material::UpdateDescriptorSets(size_t current_frame)
    {
        VkDescriptorImageInfo basic_texture_info{};
        VkWriteDescriptorSet descriptor_write{};
        std::vector<VkWriteDescriptorSet> descriptor_writes;
        if (this->basic_texture.has_value())
        {
            basic_texture_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            basic_texture_info.imageView = this->basic_texture.value()->self->texture_image_view;
            basic_texture_info.sampler = this->basic_texture.value()->self->texture_sampler;

            descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write.dstSet = this->m_descriptor_set->descriptor_sets.at(current_frame);
            descriptor_write.dstBinding = 0;
            descriptor_write.dstArrayElement = 0;
            descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_write.descriptorCount = 1;
            descriptor_write.pImageInfo = &basic_texture_info;

            descriptor_writes.push_back(descriptor_write);
        }
        vkUpdateDescriptorSets(this->p_pipeline->p_context->m_device->device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
    }

    Material::~Material()
    {

    }
}