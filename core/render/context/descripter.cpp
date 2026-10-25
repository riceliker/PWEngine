#include "core/render.hpp"
#include "core/render/_render.hpp"
#include "core/render/pipeline/_pipeline.hpp"
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::Render 
{
    std::unique_ptr<DescriptorSet> PipelineHUD::createDS()
    {
        auto obj = std::make_unique<DescriptorSet>(this);
        return obj;
    }

    DescriptorSet::DescriptorSet(PipelineHUD* pipeline) : self(std::make_unique<Impl>())
    {
        /* create DS */
        auto layout = pipeline->self->descriptor_set_layouts[0];
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);

        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = pipeline->p_context->self->descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        alloc_info.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptor_sets;

        descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);
        VkResult res = vkAllocateDescriptorSets(pipeline->p_context->m_device->device, &alloc_info, descriptor_sets.data());
        if (res != VK_SUCCESS) 
            sendLog(pipeline->p_context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate descriptor sets! vulkan return code:"+std::to_string(res));

        this->self->descriptor_sets = std::move(descriptor_sets);
    }

    void DescriptorSet::update(PipelineHUD* pipeline, Texture2D* texture)
    {
        for (size_t slot = 0; slot < MAX_FRAMES_IN_FLIGHT; slot++)
        {
            VkDescriptorImageInfo basic_texture_info{};
            VkWriteDescriptorSet descriptor_write{};
            basic_texture_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            basic_texture_info.imageView = texture->self->texture_image_view;
            basic_texture_info.sampler = texture->self->texture_sampler;

            descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write.dstSet = this->self->descriptor_sets.at(slot);
            descriptor_write.dstBinding = 0;
            descriptor_write.dstArrayElement = 0;
            descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_write.descriptorCount = 1;
            descriptor_write.pImageInfo = &basic_texture_info;

            vkUpdateDescriptorSets(pipeline->p_context->m_device->device, 1, &descriptor_write, 0, nullptr);
        }
    }

    DescriptorSet::~DescriptorSet()
    {

    }
}