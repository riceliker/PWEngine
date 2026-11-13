// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#pragma once
#include "gpu.hpp"
#include <vulkan/vulkan.h>

namespace PWEngine::GPU
{
    struct BindingLayout::Impl
    {
        VkDescriptorPool descriptor_pool;
        VkDescriptorSetLayout descriptor_set_layout;
    };

    struct Binding::Impl
    {
        std::array<VkDescriptorSet, flight_fences_index_counts> layout;
    };
}