struct Mesh3D::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Mesh3D::Vertex3D
    {
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        VkBuffer indices_buffer;
        VkDeviceMemory indices_buffer_memory;
    };
   
    struct Material::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Material::Uniform
    {
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
    };

    struct Node3D::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Camera3D::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Camera3D::Uniform
    {
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
       
    };

    std::vector<VkDescriptorSet> createDescriptorSet(Pipeline3D* pipeline, VkDescriptorSetLayout layout)
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = pipeline->p_context->self->descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        alloc_info.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> descriptor_set;

        descriptor_set.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(pipeline->p_context->m_device->device, &alloc_info, descriptor_set.data()) != VK_SUCCESS) 
            sendLog(pipeline->p_context->p_instance->log, LogType::Error, LogFrom::VulkanRender, "failed to allocate descriptor sets!");

        return descriptor_set;
    }