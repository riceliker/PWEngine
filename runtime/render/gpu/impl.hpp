#pragma once
#include "render.hpp"

namespace PWEngine::Render 
{
    struct Instance::Impl
    {
        /* Log */
        Stream::LogSystem* log;
        /* self */
        bool is_debug = false;
        VkInstance ptr;
        VkDebugUtilsMessengerEXT debug_messenger;
        std::vector<VkPhysicalDevice> adapters;
        std::vector<Device*> devices;
        /* func */
        void createInstance(InstanceInfo info);
        void getAllAdapter();
    };

    struct Device::Impl
    {
        /* owner */
        Instance* p_instance;
        VkPhysicalDevice p_adapter;
        /* manger */
        std::vector<Window*> windows;
        std::vector<RenderPass*> render_passes;
        std::vector<CommandPool*> command_pools;
        std::vector<Sync*> syncs;
        std::vector<VertexBuffer*> vertex_buffers;
        /* self */
        VkDevice ptr;
        VkQueue graphics_queue;
        void registryWindow();
    };
}