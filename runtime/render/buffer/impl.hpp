#pragma once

#include "Render.hpp"

namespace PWEngine::Render 
{
    inline VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Utils::Vertex2D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
    inline std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Utils::Vertex2D, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Utils::Vertex2D, color);

        return attributeDescriptions;
    }

    struct Mesh2D::Impl
    {
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        VkBuffer indices_buffer;
        VkDeviceMemory indices_buffer_memory;
    };

    struct Texture2D::Impl
    {
        VkImage texture_image;
        VkDeviceMemory texture_image_memory;
        VkImageView texture_image_view;
        VkSampler texture_sampler;
    };
}