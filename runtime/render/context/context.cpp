#include "render.hpp"
#include "impl.hpp"
#include "../instance/impl.hpp"
#include "check.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::Render
{
    RenderContext::RenderContext(): self(std::make_unique<Impl>()) {}

    std::shared_ptr<RenderContext> RenderInstance::createContext(ContextInfo info)
    {
        auto obj = std::make_shared<RenderContext>();
        obj->p_instance = this;
        obj->log = this->log;
        obj->m_device = std::make_unique<RenderContext::Device>(RenderContext::Device(obj.get()));
        obj->createCommandPool();
        obj->createDescriptorPool();
        obj->createSync();
        obj->m_window = std::make_unique<RenderContext::Window>(RenderContext::Window(obj.get(), info.is_window_resizable, info.window_default_resolution, info.window_title));
        obj->m_swapchain = std::make_unique<RenderContext::Swapchain>(RenderContext::Swapchain(obj.get()));
        return obj;
    }

    RenderContext::Device::Device(RenderContext* super)
    {
        VkPhysicalDevice adapter = super->p_instance->self->adapters;
        VkDevice device;
        VkQueue graphics_queue;
        QueueFamilyIndices indices = findQueueFamilies(adapter);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        
        float queue_priority = 1.0f;
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = indices.graphicsFamily.value();
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);

        VkPhysicalDeviceFeatures physics_features{};
        vkGetPhysicalDeviceFeatures(adapter, &physics_features);

        VkPhysicalDeviceFeatures need_features{};
        if (physics_features.samplerAnisotropy) need_features.samplerAnisotropy = VK_TRUE;
        

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pQueueCreateInfos = queue_create_infos.data();

        device_create_info.pEnabledFeatures = &need_features;

        device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();

        const VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feature
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = nullptr,
            .dynamicRendering = VK_TRUE,
        };
        device_create_info.pNext = &dynamic_rendering_feature;

        if (vkCreateDevice(adapter, &device_create_info, nullptr, &device) != VK_SUCCESS)
            Stream::log(super->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create logical device!");

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphics_queue);

        this->adapter = adapter;
        this->device = device;
        this->graphics_queue = graphics_queue;
    }

    RenderContext::Window::Window(RenderContext* super, bool is_window_resizable, Utils::Vec2<uint32_t> window_default_resolution, std::string window_title)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, is_window_resizable ? GLFW_TRUE : GLFW_FALSE);
        auto window = glfwCreateWindow(window_default_resolution.x, window_default_resolution.y, window_title.c_str(), nullptr, nullptr);
        
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(super->p_instance->self->instance, window, nullptr, &surface) != VK_SUCCESS)
            Stream::log(super->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create window surface!");

        this->window = window;
        this->surface = surface;
    }

    void RenderContext::createSync()
    {
        std::vector<VkSemaphore> image_available_semaphores;
        std::vector<VkSemaphore> render_finished_semaphores;
        std::vector<VkFence> in_flight_fences;

        image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(this->m_device->device, &semaphore_info, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(this->m_device->device, &semaphore_info, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(this->m_device->device, &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }

        this->self->image_available_semaphores = image_available_semaphores;
        this->self->render_finished_semaphores = render_finished_semaphores;
        this->self->in_flight_fences = in_flight_fences;
    }

    void RenderContext::createCommandPool()
    {
        VkCommandPool command_pool;
        std::vector<VkCommandBuffer> command_buffers;
        command_buffers.resize(MAX_FRAMES_IN_FLIGHT);

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(this->m_device->adapter);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(this->m_device->device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create command pool!");

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = command_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = (uint32_t) command_buffers.size();

        if (vkAllocateCommandBuffers(this->m_device->device, &alloc_info, command_buffers.data()) != VK_SUCCESS)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to allocate command buffers!");

        this->self->command_pool = command_pool;
        this->self->command_buffers = std::move(command_buffers);
    }

    void RenderContext::createDescriptorPool()
    {
        VkDescriptorPool descriptorPool;

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT * 8;

        if (vkCreateDescriptorPool(this->m_device->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        this->self->descriptor_pool = descriptorPool;
    }

    bool RenderContext::getIsClosed()
    {
        return glfwWindowShouldClose(this->m_window->window);
    }

    void RenderContext::waitIdle()
    {
        vkDeviceWaitIdle(this->m_device->device);
    }

    RenderContext::~RenderContext()
    {
        /* createSwapchain */
        for (auto image_view : this->m_swapchain->swapchain_image_views) {
            vkDestroyImageView(this->m_device->device, image_view, nullptr);
        }
        vkDestroySwapchainKHR(this->m_device->device, this->m_swapchain->swapchain, nullptr);
        /* createCommandPool */
        vkDestroyCommandPool(this->m_device->device, this->self->command_pool, nullptr);
        /* createSync */
        for (auto render_finished_semaphore: this->self->render_finished_semaphores)
        {
            vkDestroySemaphore(this->m_device->device, render_finished_semaphore, nullptr);
        }
        for (auto image_available_semaphore: this->self->image_available_semaphores)
        {
            vkDestroySemaphore(this->m_device->device, image_available_semaphore, nullptr);
        }
        for (auto in_flight_fence: this->self->in_flight_fences)
        {
            vkDestroyFence(this->m_device->device, in_flight_fence, nullptr);
        }
        for (auto& UBO : this->self->UBOs)
        {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
            {
                vkDestroyBuffer(this->m_device->device, UBO.self->uniform_buffers[i], nullptr);
                vkFreeMemory(this->m_device->device, UBO.self->uniform_buffers_memory[i], nullptr);
            }
        }
        /* addDescriptorSetLayout */
        vkDestroyDescriptorPool(this->m_device->device, this->self->descriptor_pool, nullptr);
        for (auto descriptor_set_layout : this->self->descriptor_set_layouts)
        {
            vkDestroyDescriptorSetLayout(this->m_device->device, descriptor_set_layout, nullptr);
        }
        /* createWindow */
        vkDestroySurfaceKHR(this->p_instance->self->instance, this->m_window->surface, nullptr);
        glfwDestroyWindow(this->m_window->window);
        /* createDevice */
        vkDestroyDevice(this->m_device->device, nullptr);
    }
}