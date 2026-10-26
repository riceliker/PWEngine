#include "_instance.hpp"
#include <iostream>

namespace PWEngine::GPU
{
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data)
    {
        std::cerr <<  "validation layer: " << p_callback_data->pMessage << std::endl;
        return VK_FALSE;
    }

    GPUInstance::GPUInstance(LogSystem* log, GPUInstanceInfo info)
    {
        glfwInit();
        this->self = std::make_unique<Impl>();
        this->log = log;
        this->is_debug = info.is_debug;

        this->createInstance(info);
        this->createDevice();
        this->createCommandPool();
    }
    void GPUInstance::createInstance(GPUInstanceInfo info)
    {
        VkInstance instance;

        /* application */
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = info.application_name.c_str();
        app_info.applicationVersion = VK_MAKE_VERSION(info.application_version.x, info.application_version.y, info.application_version.z);
        app_info.pEngineName = "PWEngine";
        app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        app_info.apiVersion = VK_API_VERSION_1_3;

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
                    sendLog(this->log, LogType::Warn, LogFrom::VulkanRender, "validation layers requested, but not available!");
                    this->is_debug = false;
                }
            }

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
            sendLog(this->log, LogType::Error, LogFrom::VulkanRender, "failed to create instance!");

        this->self->instance = instance;
    }

    GPUInstance::~GPUInstance()
    {
        if (this->is_debug) 
        {
            destroyDebugUtilsMessengerEXT(this->self->instance, this->self->debug_messenger, nullptr);
        }
        vkDestroyInstance(this->self->instance, nullptr);
    }

} // namespace PWEngine::Render