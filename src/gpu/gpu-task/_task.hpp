// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#pragma once
#include <vulkan/vulkan.h>
#include "gpu.hpp"

namespace PWEngine::GPU 
{
    struct GPUTask::Impl
    {
        VkSemaphore compute_finished_semaphore;
        VkFence in_flight_fence;

        VkCommandPool command_pool;
        VkCommandBuffer command_buffer;
    };
}