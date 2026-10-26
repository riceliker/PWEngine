// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#pragma once
#include <vulkan/vulkan.h>
#include "gpu.hpp"

namespace PWEngine::GPU
{
    struct Pipeline::Impl
    {
        VkPipelineLayout pipeline_layout;
        VkPipeline graphics_pipeline;

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
    };

    inline std::optional<VkShaderModule> createShaderModule(std::variant<std::vector<char>, std::vector<uint32_t>> code, VkDevice device) {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        std::visit([&](auto& arg){
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::vector<uint32_t> >) 
            {
                create_info.codeSize = 4 * arg.size();
                create_info.pCode = arg.data();
            }
            else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::vector<char>>) 
            {
                create_info.codeSize = arg.size();
                create_info.pCode = reinterpret_cast<const uint32_t*>(arg.data());
            }
        }, code);
        
        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) 
        {
            return std::nullopt;
        }   

        return shader_module;
    }
}