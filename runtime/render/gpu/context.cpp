#include "render.hpp"
#include "impl.hpp"
#include "check.hpp"
#include <memory>
#include <vector>

namespace PWEngine::Render
{
    RenderContext::RenderContext(): self(std::make_unique<Impl>())
    {

    }

    RenderContext* Instance::createContext(WindowInfo info)
    {
        RenderContext* obj = new RenderContext();
        obj->p_instance = this;
        obj->log = this->log;
        obj->createDevice();
        obj->createCommandPool();
        obj->createSync();
        obj->createWindow(info);
        this->context_list.push_back(obj);
        obj->index = context_list.size();
        return obj;
    }

    void RenderContext::createDevice()
    {
        VkPhysicalDevice adapter = this->p_instance->self->adapters[0];
        this->self->adapter = this->p_instance->self->adapters[0];
        /* make device*/
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

        VkPhysicalDeviceFeatures device_features{};

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pQueueCreateInfos = queue_create_infos.data();

        device_create_info.pEnabledFeatures = &device_features;

        device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();

        if (vkCreateDevice(adapter, &device_create_info, nullptr, &device) != VK_SUCCESS)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create logical device!");

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphics_queue);

        this->self->device = device;
        this->self->graphics_queue = graphics_queue;
    }

    void RenderContext::createWindow(WindowInfo info)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, info.is_resizable ? GLFW_TRUE : GLFW_FALSE);
        auto window = glfwCreateWindow(info.size.x, info.size.y, info.title.c_str(), nullptr, nullptr);
        
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(this->p_instance->self->instance, window, nullptr, &surface) != VK_SUCCESS)
            Stream::log(this->p_instance->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create window surface!");

        this->self->window = window;
        this->self->surface = surface;
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
            if (vkCreateSemaphore(this->self->device, &semaphore_info, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(this->self->device, &semaphore_info, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(this->self->device, &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {
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
        command_buffers.resize(8);

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(this->self->adapter);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(this->self->device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create command pool!");

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = command_pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) command_buffers.size();

        if (vkAllocateCommandBuffers(this->self->device, &allocInfo, command_buffers.data()) != VK_SUCCESS)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to allocate command buffers!");

        this->self->command_pool = command_pool;
        this->self->command_buffers = std::move(command_buffers);
    }

    bool RenderContext::getIsClosed()
    {
        return glfwWindowShouldClose(this->self->window);
    }

    void RenderContext::waitIdle()
    {
        vkDeviceWaitIdle(this->self->device);
    }

    RenderContext::~RenderContext()
    {
        /* createSwapchain */
        for (auto framebuffer : this->self->swapchain_framebuffers) {
            vkDestroyFramebuffer(this->self->device, framebuffer, nullptr);
        }
        for (auto image_view : this->self->swapchain_image_views) {
            vkDestroyImageView(this->self->device, image_view, nullptr);
        }
        vkDestroySwapchainKHR(this->self->device, this->self->swapchain, nullptr);
        /* createCommandPool */
        vkDestroyCommandPool(this->self->device, this->self->command_pool, nullptr);
        /* createSync */
        for (auto render_finished_semaphore: this->self->render_finished_semaphores)
        {
            vkDestroySemaphore(this->self->device, render_finished_semaphore, nullptr);
        }
        for (auto image_available_semaphore: this->self->image_available_semaphores)
        {
            vkDestroySemaphore(this->self->device, image_available_semaphore, nullptr);
        }
        for (auto in_flight_fence: this->self->in_flight_fences)
        {
            vkDestroyFence(this->self->device, in_flight_fence, nullptr);
        }
        /* createWindow */
        vkDestroySurfaceKHR(this->p_instance->self->instance, this->self->surface, nullptr);
        glfwDestroyWindow(this->self->window);
        /* createDevice */
        vkDestroyDevice(this->self->device, nullptr);
    }
}