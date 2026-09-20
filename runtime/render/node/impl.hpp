#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
#include "render.hpp"
#include <vector>

namespace PWEngine::Render 
{
    template<typename T>
    inline VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(T);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    template<typename T>
    inline std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(T, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(T, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(T, uv);

        return attributeDescriptions;
    }

    struct Texture2D::Impl
    {
        uint32_t mip_level;
        VkImage texture_image;
        VkDeviceMemory texture_image_memory;
        VkImageView texture_image_view;
        VkSampler texture_sampler;
    };

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

    struct Mesh3D::Uniform
    {
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
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

    struct Camera::DescriptorSet
    {
        std::vector<VkDescriptorSet> descriptor_sets;
    };

    struct Camera::Uniform
    {
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        std::vector<void*> uniform_buffers_mapped;
       
    };
}   