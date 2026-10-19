// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
#pragma once
#include "format.hpp"
#include "core/stream.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File 
{  
    std::unique_ptr<ImageRGBA8> tgaReader(LogSystem* log, std::string load_path);
    std::unique_ptr<Model3D> objReader(LogSystem* log, std::string load_path);

    std::optional<std::vector<char>> shaderReader(LogSystem* log, std::string file_path);
}