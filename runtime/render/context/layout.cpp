#include "render.hpp"
#include "impl.hpp"

namespace PWEngine::Render 
{
    size_t RenderContext::addModelDescriptorSetLayout()
    {
        VkDescriptorSetLayout descriptorSetLayout;

        /* Vertex */
        VkDescriptorSetLayoutBinding model_layout_binding{};
        model_layout_binding.binding = 0;
        model_layout_binding.descriptorCount = 1;
        model_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        model_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        /* Texture */
        VkDescriptorSetLayoutBinding texture_layout_binding{};
        texture_layout_binding.binding = 1;
        texture_layout_binding.descriptorCount = 1;
        texture_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texture_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


        std::vector<VkDescriptorSetLayoutBinding> bindings = {model_layout_binding, texture_layout_binding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(this->m_device->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        this->self->descriptor_set_layouts.push_back(descriptorSetLayout);
        return this->self->descriptor_set_layouts.size() - 1;
    }

    size_t RenderContext::addCameraDescriptorSetLayout()
    {
        VkDescriptorSetLayout descriptorSetLayout;

        VkDescriptorSetLayoutBinding camera_layout_binding{};
        camera_layout_binding.binding = 0;
        camera_layout_binding.descriptorCount = 1;
        camera_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        camera_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        std::vector<VkDescriptorSetLayoutBinding> bindings = {camera_layout_binding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(this->m_device->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        this->self->descriptor_set_layouts.push_back(descriptorSetLayout);
        return this->self->descriptor_set_layouts.size() - 1;
    }
}