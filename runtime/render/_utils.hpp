#pragma once
/* include library */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif
/* header */
#include "render.hpp"
#include "render/_render.hpp"
/* std library */
#include <cstdint>

namespace PWEngine::Render 
{
/*
    ██  ░███    ███   ███████  ██████████  ████     ███    ░██    ███████    █████████
    ██  ░████   ███  ██    ███    ▓██      ████▓    ████░  ░██  ▓██▒    ██▓  ██
    ██  ░██ ██  ███  █████▓       ▓██     ██  ██    ██░███ ░██  ███          ████████
    ██  ░██  ██▓███     ░█████    ▓██    ███  ▓██   ██░ ███░██  ██▓          ██
    ██  ░██   █████ ███     ██    ▓██   ██████████  ██░  ▒████  ███     ██▓  ██
    ██  ░██    ▓███  ▓███████     ▓██  ░██      ███ ██░    ███    ███████    █████████
*/
    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_create_info, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger);
    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* p_allocator);
/*
     ███████     ███████░   ███░    ██ ▓█████████ █████████ ▓██▒   ███ █████████
    ███    ███  ███    ███  █████   ██     ██▒    ███         ███ ██▒     ███
   ███         ███      ██▒ ██████  ██     ██▒    ████████▒    ████       ███
   ███         ███      ██▒ ███ ▓██ ██     ██▒    ███          ████▒      ███
    ██▓    ███  ███    ███  ███   ████     ██▒    ███        ▒██░ ███     ███
     ███████     ███████▓   ███    ███     ██▒    █████████ ███    ███    ███
*/
    const std::vector<const char*> device_extensions = {
    #if (__APPLE__)
        VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    #endif
        /* swapchain */
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        /* dynamic rendering */
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };
    
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value();
        }
    };

    struct swapchain_supportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
    swapchain_supportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

/*
   ▒███    ██    ███████    ████████    █████████
   ▒████   ██   ███    ███  ███    ███  ███
   ▒██▒██  ██  ███      ██░ ███     ██  ████████
   ▒██  ██ ██  ███      ██░ ███     ██  ███
   ▒██   ████   ██▓    ███  ███    ███  ███
   ▒██    ███    ███████░   ████████░   █████████
*/ 
    template<typename T> inline VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(T);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    template<typename T> inline std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
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

    uint32_t findMemoryType(RenderContext* context, uint32_t type_filter, VkMemoryPropertyFlags properties);
    void createStagingBuffer(RenderContext* context, size_t size, VkBuffer& buffer, VkDeviceMemory& memory);
    void createRealBuffer(RenderContext* context, size_t size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory);
    void createDirectBuffer(RenderContext* context, size_t size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory);
    void createImageBuffer(RenderContext* context, Vec2<uint32_t> size, uint32_t mip_level, VkImage& image, VkDeviceMemory& memory);
    void createDepthBuffer(RenderContext* context, VkExtent2D swapchain_extent, VkImage& image, VkDeviceMemory& memory);
    std::vector<VkDescriptorSet> createDescriptorSet(Pipeline3D* pipeline, VkDescriptorSetLayout layout);
/*
     ███████     ███████    ████     ████  ████    ████     ████    ░███    ██  ▒████████
    ███    ██▒  ███    ███  █████   █████  ████░   ████    ░████    ░████   ██  ▒██    ███
   ███         ███      ██  ██▓██  ▓█████  ██ ██  ██░██    ██ ░██   ░██▒██  ██  ▒██     ███
   ███         ███      ██  ██▓ ██ ██ ███  ██ ███▒██ ██   ███  ███  ░██  ██ ██  ▒██     ███
   ░██▒    ██▒  ██▓    ███  ██▓ ████  ███  ██  ████  ██  █████████▓ ░██   ████  ▒██    ███
     ███████     ███████    ██▓  ███  ███  ██   ██   ██ ░██     ░██ ░██    ███  ▒████████
*/
    void copyBufferCommand(RenderContext* context, VkBuffer staging, VkBuffer real, size_t size);
    void copyBufferToImage(RenderContext* context, VkBuffer buffer, VkImage image, Vec2<uint32_t> size);
    void transitionImageLayout(RenderContext* context, uint32_t min_level, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void generateMipmap(RenderContext* context, uint32_t mip_level, Vec2<uint32_t> size, VkImage image);
}