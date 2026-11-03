#include "gpu.hpp"
#include "gpu/instance/_instance.hpp"
#include "_binding.hpp"

#include "stream.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
namespace PWEngine::GPU
{
    BindingLayout::BindingLayout(GPUInstance* instance, std::vector<BindingInfo> binding_info) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;
        this->binding_infos = std::move(binding_info);
        std::fill(this->binding_type_count.begin(), this->binding_type_count.end(), 8);
        this->createPool();
        this->createLayout();
    }

    BindingLayout::~BindingLayout()
    {
        
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
            case BindingType::image_sampler:
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

        pool_sizes[6].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[6].descriptorCount = flight_fences_index_counts * this->binding_type_count[6];

        pool_sizes[7].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        pool_sizes[7].descriptorCount = flight_fences_index_counts * this->binding_type_count[7];

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        poolInfo.pPoolSizes = pool_sizes.data();
        poolInfo.maxSets = 8;

        if (vkCreateDescriptorPool(this->p_instance->self->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
            sendLog(this->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create descriptor pool!");

        this->self->descriptor_pool = descriptorPool;
    }

    void BindingLayout::createLayout()
    {
        VkDescriptorSetLayout descriptor_set_layout;
        
        std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
        for (const auto& buffer_info : this->binding_infos)
        {
            VkDescriptorSetLayoutBinding binding{};
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
            case BindingType::image_sampler:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                binding.pImmutableSamplers = nullptr;
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
            break;
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
}