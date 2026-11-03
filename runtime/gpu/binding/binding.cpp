#include "_binding.hpp"
#include "gpu/instance/_instance.hpp"
#include "gpu/allocator/_allocator.hpp"
#include "gpu.hpp"
#include <array>
#include <memory>

namespace PWEngine::GPU 
{
    std::unique_ptr<Binding> BindingLayout::createBinding()
    {
        auto obj = std::make_unique<Binding>(this);
        return obj;
    }

    Binding::Binding(BindingLayout* layout) : self(std::make_unique<Impl>()) 
    {
        this->p_layout = layout;
        std::array<VkDescriptorSetLayout, flight_fences_index_counts> layouts({});
        std::fill(layouts.begin(), layouts.end(), layout->self->descriptor_set_layout);

        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = layout->self->descriptor_pool;
        alloc_info.descriptorSetCount = flight_fences_index_counts;
        alloc_info.pSetLayouts = layouts.data();

        std::array<VkDescriptorSet, flight_fences_index_counts> descriptor_sets;

        VkResult res = vkAllocateDescriptorSets(layout->p_instance->self->device, &alloc_info, descriptor_sets.data());
        if (res != VK_SUCCESS) 
            sendLog(layout->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate descriptor sets! vulkan return code:"+std::to_string(res));

        this->self->layout = std::move(descriptor_sets);
    }

    Binding::~Binding()
    {

    }

    void Binding::update(uint32_t start_offset, std::vector<std::variant<GPUBinding*, GPUImageView*>> objects)
    {
        for (size_t slot = 0; slot < flight_fences_index_counts; slot++)
        {
            std::vector<VkDescriptorBufferInfo> buffer_info{};
            std::vector<VkDescriptorImageInfo> image_info{};
            for (uint32_t i = start_offset; i < start_offset + objects.size(); ++i)
            {
                std::visit([this, slot, i](auto& arg){
                    VkWriteDescriptorSet descriptor_write{};
                    VkDescriptorBufferInfo buffer_info{};
                    VkDescriptorImageInfo image_info{};
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, GPUBinding*>) 
                    {
                        buffer_info.buffer = arg->self->buffer;
                        buffer_info.offset = 0;
                        buffer_info.range = VK_WHOLE_SIZE;
                        descriptor_write.pBufferInfo = &buffer_info;
                    } else {
                        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        image_info.imageView = arg->self->view;
                        image_info.sampler = arg->self->sampler;
                        descriptor_write.pImageInfo = &image_info;
                    }
                    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptor_write.dstSet = this->self->layout.at(slot);
                    descriptor_write.dstBinding = i;
                    descriptor_write.dstArrayElement = 0;
                    descriptor_write.descriptorCount = 1;
                    switch (this->p_layout->binding_infos.at(i).binding_type) 
                    {
                    case BindingType::uniform:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    break;
                    case BindingType::uniform_dynamic:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                    break;
                    case BindingType::uniform_texel:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                    break;
                    case BindingType::storage:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;
                    case BindingType::storage_dynamic:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                    break;
                    case BindingType::storage_texel:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
                    break;
                    case BindingType::image_sampler:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                    case BindingType::sampled_image:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    break;
                    }  
                    
                    vkUpdateDescriptorSets(this->p_layout->p_instance->self->device, 1, &descriptor_write, 0, nullptr);
                }, objects[i]);
            }            
        }
    }
}