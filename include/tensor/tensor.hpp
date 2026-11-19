// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#include "gpu.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace PWEngine::ML
{
    struct Tensor1
    {
        std::unique_ptr<GPU::GPULocalBuffer> buffer;
        std::vector<uint16_t> shape;
        GPU::GPUInstance* p_instance;
        Tensor1(GPU::GPUInstance* instance, std::vector<float> data);
        void reshape(std::vector<uint16_t> shape);
    };
}