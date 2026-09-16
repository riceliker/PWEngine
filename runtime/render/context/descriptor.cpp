#include "render.hpp"
#include "impl.hpp"
#include "../buffer/impl.hpp"
#include <cstddef>
#include <memory>

namespace PWEngine::Render
{
    DescriptorSet::DescriptorSet():self(std::make_unique<Impl>()){}
    DescriptorSet::~DescriptorSet()
    {
        
    }
    std::unique_ptr<DescriptorSet> RenderContext::createDescriptorSet(size_t descriptor_set_layout_index)
    {
        VkDescriptorSetLayout descriptor_set_layout = this->self->descriptor_set_layouts[descriptor_set_layout_index];
        
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptor_set_layout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->self->descriptor_pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptor_sets;

        descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(this->m_device->device, &allocInfo, descriptor_sets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        auto obj = std::make_unique<DescriptorSet>();
        obj->p_context = this;
        obj->self->descriptor_sets = std::move(descriptor_sets);
        return obj;
    }

}