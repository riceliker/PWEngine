#pragma once
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#if(__APPLE__)
#include <vulkan/vulkan_beta.h>
#endif

#include <string>

#include "Utils.hpp"

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
    GLFWwindow* ptr;
public:
    Window(WindowInfo info);
};
/*
----- GPU Module -----
The GPU module is control the vulkan instance with C++ class.
Use it is simple, you should not write the template code.


*/
namespace GPU
{
    struct InstanceInfo
    {
        std::string name;
        Utils::Vec3<uint8_t> version;
        bool is_debug;
    };
    class Instance
    {
    private:
        VkInstance ptr;
    public:
        Instance(InstanceInfo info);
    };

    /* Adapter
        The Physical Device in Vulkan. An adapter mean a real GPU device.
    */
    class Adapter
    {
    private:
        VkPhysicalDevice ptr;
    };

    class AdapterFactory
    {
    public:
        AdapterFactory();
    };

    class Device
    {
    private:
        VkDevice ptr;
    };
}







}