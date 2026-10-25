
// static inline void createDescriptorPool(RenderContext* context)
//     {
//         VkDescriptorPool descriptorPool;

//         std::array<VkDescriptorPoolSize, 3> pool_sizes{};
//         pool_sizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//         pool_sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//         pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//         pool_sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//         pool_sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         pool_sizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

//         VkDescriptorPoolCreateInfo poolInfo{};
//         poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//         poolInfo.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
//         poolInfo.pPoolSizes = pool_sizes.data();
//         poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT * 8;

//         if (vkCreateDescriptorPool(context->m_device->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
//             sendLog(context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to create descriptor pool!");

//         context->self->descriptor_pool = descriptorPool;
//     }