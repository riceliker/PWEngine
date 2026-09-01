#pragma once
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif

#include <string>
#include <vector>

#include "utils.hpp"

/*
----- GPU Module -----
The GPU module is control the vulkan instance with C++ class.
Use it is simple, you should not write the template code.

Window->Instance->Adapter->Device->

*/
namespace PWEngine::Render 
{
    class Window;
}


namespace PWEngine::Render::GPU
{
    
    /* Adapter
        The Physical Device in Vulkan. An adapter mean a real GPU device.
    */
    class Adapter
    {
    private:
        VkPhysicalDevice ptr;
    };

    class Device
    {
    private:
        VkDevice ptr;
    };

    struct InstanceInfo
    {
        std::string name;
        Utils::Vec3<uint8_t> version;
        bool is_debug;
    };
    class Instance
    {
    private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        VkSurfaceKHR surface;
        std::vector<VkPhysicalDevice> devices;
    public:
        Instance(InstanceInfo info, Window window);
        Adapter GetBestAdapter();
    };
}

namespace PWEngine::Render 
{
    struct WindowInfo
    {
        Utils::Vec2<uint32_t> size;
        std::string title;
        bool is_resizable;
    };
    class Window
    {
    private:
        GLFWwindow* window;
        
    public:
        Window(WindowInfo info);
        friend class GPU::Instance;
    };
}
