#include "gpu.hpp"

#include <vulkan/vulkan.h>

#include <vector>
namespace PWEngine::GPU
{
    struct BindingLayout::Impl
    {
        VkDescriptorPool descriptor_pool;
        VkDescriptorSetLayout descriptor_set_layout;
        std::vector<std::vector<VkDescriptorSet>> descriptor_sets_list;
    };
}