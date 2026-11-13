// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#pragma once
#include "gpu.hpp"
#include <vulkan/vulkan.h>

namespace PWEngine::GPU 
{
    struct ResultImage::Impl
    {
        VkImage result_image;
        VkImage result_depth_image;
        VkImageView result_image_view;
        VkImageView result_depth_image_view;
        Vec2<uint32_t> result_image_size;
    };

}