#include "render.hpp"
#include "impl.hpp"
#include "stream.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::Render
{
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    static inline VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    /*
        ██░ ▓███    ████ ▒███████  ██▒
        ██░ ▓████  █████ ▒██   ▒██ ██▒
        ██░ ▓██▒██▓█▒███ ▒███████  ██▒
        ██░ ▓██ ████ ███ ▒██       ██▒
        ██░ ▓██  ██  ███ ▒██       ███████▒
    */

    bool Instance::checkValidationLayer()
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

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
                Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "validation layers requested, but not available!");
                return false;
            }
        }
        return true;
    }

    void Instance::createInstance(InstanceInfo info)
    {
        VkInstance instance;

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
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

        if (info.is_debug)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
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
       
        if (info.is_debug)
        {
            instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            instance_create_info.ppEnabledLayerNames = validation_layers.data();

            VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
            debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debug_create_info.pfnUserCallback = debugCallback;

            instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
        }
        else
        {
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.pNext = nullptr;
        }
        /* make instance */
        if (vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to create instance!");

        this->self->instance = instance;
    }

    void Instance::getBestAdapter()
    {
        /* find all physics device */
        uint32_t adapter_count = 0;
        vkEnumeratePhysicalDevices(this->self->instance, &adapter_count, nullptr);

        if (adapter_count == 0)
            Stream::log(this->log, Stream::LogType::Error, Stream::LogFrom::VulkanRender, "failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> adapters(adapter_count);
        vkEnumeratePhysicalDevices(this->self->instance, &adapter_count, adapters.data());

        this->self->adapters = adapters;
    }

    Instance::Instance(InstanceInfo info, Stream::LogSystem* log): self(std::make_unique<Impl>())
    {
        glfwInit();

        bool is_debug = info.is_debug;
        if (is_debug)
            is_debug = this->checkValidationLayer();

        this->createInstance(info);

        this->getBestAdapter();
    }

    static inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    Instance::~Instance()
    {
        for (const auto& context: this->context_list)
        {
            delete context;
        }
        if (this->self->is_debug) 
        {
            DestroyDebugUtilsMessengerEXT(this->self->instance, this->self->debug_messenger, nullptr);
        }
        vkDestroyInstance(this->self->instance, nullptr);
    }

} // namespace PWEngine::Render