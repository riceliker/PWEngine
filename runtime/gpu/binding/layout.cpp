#include "gpu.hpp"
#include "gpu/instance/_instance.hpp"
#include "gpu/allocator/_allocator.hpp"
#include "_binding.hpp"

#include "stream.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <variant>
#include <vector>
namespace PWEngine::GPU
{
    BindingLayout::BindingLayout(GPUInstance* instance, std::vector<BindingInfo> binding_info)
    {
        this->p_instance = instance;
        this->binding_infos = std::move(binding_info);
        this->createPool();
        this->createLayout();
        this->allocateBinding();
    }

    void BindingLayout::createPool()
    {
        for (const auto& buffer_info : this->binding_infos)
        {
            switch (buffer_info.binding_type) 
            {
            case BindingType::uniform:
                this->binding_type_count[0] += 1;
            break;
            case BindingType::uniform_dynamic:
                this->binding_type_count[1] += 1;
            break;
            case BindingType::uniform_texel:
                this->binding_type_count[2] += 1;
            break;
            case BindingType::storage:
                this->binding_type_count[3] += 1;
            break;
            case BindingType::storage_dynamic:
                this->binding_type_count[4] += 1;
            break;
            case BindingType::storage_texel:
                this->binding_type_count[5] += 1;
            break;
            case BindingType::sampler:
                this->binding_type_count[6] += 1;
            break;
            case BindingType::sampled_image:
                this->binding_type_count[7] += 1;
            break;
            }
        }

        VkDescriptorPool descriptorPool;
        std::array<VkDescriptorPoolSize, 8> pool_sizes{};

        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = flight_fences_index_counts * this->binding_type_count[0];

        pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        pool_sizes[1].descriptorCount = flight_fences_index_counts * this->binding_type_count[1];

        pool_sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        pool_sizes[2].descriptorCount = flight_fences_index_counts * this->binding_type_count[2];

        pool_sizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        pool_sizes[3].descriptorCount = flight_fences_index_counts * this->binding_type_count[3];

        pool_sizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        pool_sizes[4].descriptorCount = flight_fences_index_counts * this->binding_type_count[4];

        pool_sizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        pool_sizes[5].descriptorCount = flight_fences_index_counts * this->binding_type_count[5];

        pool_sizes[6].type = VK_DESCRIPTOR_TYPE_SAMPLER;
        pool_sizes[6].descriptorCount = flight_fences_index_counts * this->binding_type_count[6];

        pool_sizes[7].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        pool_sizes[7].descriptorCount = flight_fences_index_counts * this->binding_type_count[7];

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        poolInfo.pPoolSizes = pool_sizes.data();
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(this->p_instance->self->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create descriptor pool!");
    }

    void BindingLayout::createLayout()
    {
        VkDescriptorSetLayout descriptor_set_layout;
        
        std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
        for (const auto& buffer_info : this->binding_infos)
        {
            VkDescriptorSetLayoutBinding binding;
            binding.binding = buffer_info.binding_index;
            binding.descriptorCount = buffer_info.binding_array_count;
            switch (buffer_info.binding_type) 
            {
            case BindingType::uniform:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
            case BindingType::uniform_dynamic:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            break;
            case BindingType::uniform_texel:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
            break;
            case BindingType::storage:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
            case BindingType::storage_dynamic:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            break;
            case BindingType::storage_texel:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            break;
            case BindingType::sampler:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            break;
            case BindingType::sampled_image:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            break;
            }

            switch (buffer_info.shader_type) 
            {
            case ShaderType::Vertex:
                binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            break;
            case ShaderType::Fragment:
                binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
            case ShaderType::Compute:
                binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            }

            layout_bindings.push_back(binding);
        }

        VkDescriptorSetLayoutCreateInfo descriptor_set{};
        descriptor_set.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set.bindingCount = static_cast<uint32_t>(layout_bindings.size());
        descriptor_set.pBindings = layout_bindings.data();

        if (vkCreateDescriptorSetLayout(this->p_instance->self->device, &descriptor_set, nullptr, &descriptor_set_layout) != VK_SUCCESS)
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create descriptor set layout!");
        this->self->descriptor_set_layout = descriptor_set_layout;  
    }

    void BindingLayout::allocateBinding()
    {
        for (const auto& buffer_info : this->binding_infos)
        {
            std::vector<VkDescriptorSetLayout> layouts(flight_fences_index_counts, this->self->descriptor_set_layout);

            VkDescriptorSetAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            alloc_info.descriptorPool = this->self->descriptor_pool;
            alloc_info.descriptorSetCount = flight_fences_index_counts;
            alloc_info.pSetLayouts = layouts.data();

            std::vector<VkDescriptorSet> descriptor_sets;

            descriptor_sets.resize(flight_fences_index_counts);
            VkResult res = vkAllocateDescriptorSets(this->p_instance->self->device, &alloc_info, descriptor_sets.data());
            if (res != VK_SUCCESS) 
                sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate descriptor sets! vulkan return code:"+std::to_string(res));

            this->self->descriptor_sets_list.push_back(std::move(descriptor_sets));
        }
    }

    void BindingLayout::update(size_t index, uint32_t start_offset, std::vector<std::variant<std::tuple<GPUBuffer*, Vec2<uint32_t>>, GPUImageView*>> objects)
    {
        for (size_t slot = 0; slot < flight_fences_index_counts; slot++)
        {
            std::vector<VkDescriptorBufferInfo> buffer_info;
            std::vector<VkDescriptorImageInfo> image_info;
            for (uint32_t i = start_offset; i < start_offset + objects.size(); ++i)
            {
                std::visit([&](auto& arg){
                    VkWriteDescriptorSet descriptor_write{};
                    VkDescriptorBufferInfo buffer_info{};
                    VkDescriptorImageInfo image_info{};
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::tuple<GPUBuffer*, Vec2<uint32_t>>>) 
                    {
                        auto [buffer, area] = (std::tuple<GPUBuffer*, Vec2<uint32_t>>)arg;
                        buffer_info.buffer = buffer->self->buffer;
                        buffer_info.offset = area.x;
                        buffer_info.range = area.y;
                        descriptor_write.pBufferInfo = &buffer_info;
                    } else {
                        GPUImageView* arg = arg;
                        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        image_info.imageView = arg->self->view;
                        image_info.sampler = arg->self->sampler;
                        descriptor_write.pImageInfo = &image_info;
                    }
                    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptor_write.dstSet = this->self->descriptor_sets_list.at(index).at(slot);
                    descriptor_write.dstBinding = i;
                    descriptor_write.dstArrayElement = 0;
                    descriptor_write.descriptorCount = 1;
                    switch (this->binding_infos[index].binding_type) 
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
                    case BindingType::sampler:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                    break;
                    case BindingType::sampled_image:
                        descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    break;
                    }  
                    
                    vkUpdateDescriptorSets(this->p_instance->self->device, 1, &descriptor_write, 0, nullptr);
                }, objects[i]);
            }            
        }
    }
}