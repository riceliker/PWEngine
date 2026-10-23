#include "core/render.hpp"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include <cstdint>
#include <optional>
#include <vector>
#include <fstream>
namespace PWEngine::Render
{
    std::optional<std::variant<std::vector<char>, std::vector<uint32_t>>> openShader(std::string file_path)
    {
        std::ifstream file(file_path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) 
        {
            return std::nullopt;
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return std::move(buffer);
    }
    std::optional<std::variant<std::vector<char>, std::vector<uint32_t>>> compilerShader(const std::string& source, ShaderType type, const std::string& name)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        

        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
        options.SetSourceLanguage(shaderc_source_language_glsl);

        shaderc_shader_kind kind;
        switch (type)
        {
        case ShaderType::Vertex:
            kind = shaderc_glsl_vertex_shader;
        break;
        case ShaderType::Fragment:
            kind = shaderc_glsl_fragment_shader;
        break;
        case ShaderType::Compute:
            kind = shaderc_glsl_compute_shader;
        break;
        }
    
        const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, name.c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) 
        {
            printf("%s", result.GetErrorMessage().c_str());
            return std::nullopt;
        }

        std::vector<uint32_t> spv = {result.begin(), result.end()};

        return spv;
    }

    inline std::optional<VkShaderModule> createShaderModule(const std::vector<uint32_t>& code, VkDevice device) {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = 4 * code.size();
        create_info.pCode = code.data();

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
            return std::nullopt;
        }

        return shader_module;
    }

    inline std::optional<VkShaderModule> createShaderModule(const std::vector<char>& code, VkDevice device) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shader_module) != VK_SUCCESS) {
            return std::nullopt;
        }

        return shader_module;
    }
}
