#include "render.hpp"
#include "stream.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace PWEngine::Render
{
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"};

    static inline std::vector<const char*> GetRequiredExtensions(bool is_debug)
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(
            glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (is_debug)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage
                  << std::endl;

        return VK_FALSE;
    }

    static inline void PopulateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& create_info)
    {
        create_info = {};
        create_info.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debugCallback;
    }

    static inline VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static inline void CheckValidationLayer(Stream::LogSystem* log)
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count,
                                           available_layers.data());

        for (const char* layer_name : validation_layers)
        {
            bool layer_found = false;

            for (const auto& layer_properties : available_layers)
            {
                if (strcmp(layer_name, layer_properties.layerName) == 0)
                {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found)
            {
                Stream::log(log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "validation layers requested, but not available!");
            }
        }
    }

    /*
        ██░ ▓███    ████ ▒███████  ██▒
        ██░ ▓████  █████ ▒██   ▒██ ██▒
        ██░ ▓██▒██▓█▒███ ▒███████  ██▒
        ██░ ▓██ ████ ███ ▒██       ██▒
        ██░ ▓██  ██  ███ ▒██       ███████▒
    */

    Instance::Instance(InstanceInfo info, Stream::LogSystem* log)
    {
        this->log = log; 
        glfwInit();
        createInstance(info);
        getAllAdapter();
    }

    void Instance::createInstance(InstanceInfo info)
    {
        /* create vulkan instance */
        VkInstance instance;

        if (info.is_debug)
            CheckValidationLayer(this->log);
        /* application */
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = info.name.c_str();
        app_info.applicationVersion =
            VK_MAKE_VERSION(info.version.x, info.version.y, info.version.z);
        app_info.pEngineName = "PWEngine";
        app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instance_create_info{};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &app_info;
        /* vulkan extensions */
        auto extensions = GetRequiredExtensions(info.is_debug);
        #if (__APPLE__)
            extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            extensions.emplace_back(
                VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            instance_create_info.flags |=
                VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        #endif
        instance_create_info.enabledExtensionCount =
            static_cast<uint32_t>(extensions.size());
        instance_create_info.ppEnabledExtensionNames = extensions.data();
        /* debug layer */
        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        if (info.is_debug)
        {
            instance_create_info.enabledLayerCount =
                static_cast<uint32_t>(validation_layers.size());
            instance_create_info.ppEnabledLayerNames = validation_layers.data();

            PopulateDebugMessengerCreateInfo(debug_create_info);
            instance_create_info.pNext =
                (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
        }
        else
        {
            instance_create_info.enabledLayerCount = 0;

            instance_create_info.pNext = nullptr;
        }
        /* make instance */
        if (vkCreateInstance(&instance_create_info, nullptr, &instance) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
        /* make debug layer */
        if (info.is_debug)
        {
            VkDebugUtilsMessengerCreateInfoEXT debug_util_message_create_info;
            PopulateDebugMessengerCreateInfo(debug_util_message_create_info);

            if (CreateDebugUtilsMessengerEXT(instance,
                                             &debug_util_message_create_info,
                                             nullptr,
                                             &debug_messenger) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }

        this->instance = instance;
    }

    void Instance::getAllAdapter()
    {
        /* find all physics device */
        uint32_t adapter_count = 0;
        vkEnumeratePhysicalDevices(instance, &adapter_count, nullptr);

        if (adapter_count == 0)
        {
            throw std::runtime_error(
                "failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(adapter_count);
        vkEnumeratePhysicalDevices(instance, &adapter_count, devices.data());
                
        this->adapters = devices;
    }

    Instance::~Instance()
    {

    }

} // namespace PWEngine::Render