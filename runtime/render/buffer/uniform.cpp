#include "render.hpp"
#include "../gpu/impl.hpp"
#include "../buffer/buffer.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace PWEngine::Render 
{
    UBO::UBO() : self(std::make_unique<Impl>())
    {
        
    }
    UBO::~UBO()
    {

    }
    std::unique_ptr<UBO> RenderContext::createUBO(size_t descriptor_set_layout_index)
    {
        VkDeviceSize bufferSize = sizeof(UBO::UBOData);

        std::unique_ptr<UBO> obj = std::make_unique<UBO>();
        obj->p_context = this;
        obj->self->uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        obj->self->uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
        obj->self->uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            createDirectBuffer(this, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, obj->self->uniform_buffers[i], obj->self->uniform_buffers_memory[i]);
            vkMapMemory(this->self->device, obj->self->uniform_buffers_memory[i], 0, bufferSize, 0, &obj->self->uniform_buffers_mapped[i]);
        }

        VkDescriptorSetLayout layout = this->self->descriptor_set_layouts[descriptor_set_layout_index];

        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->self->descriptor_pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptorSets;

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(this->self->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer =  obj->self->uniform_buffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UBO::UBOData);
            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional
            vkUpdateDescriptorSets(this->self->device, 1, &descriptorWrite, 0, nullptr);
        }
        
        obj->self->descriptor_sets = std::move(descriptorSets);
        return obj;
    }
}