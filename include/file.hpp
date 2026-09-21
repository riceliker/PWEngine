#pragma once
#include "stream.hpp"
#include "utils.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File 
{  
    std::unique_ptr<Utils::ImageRGBA8> tgaReader(Stream::LogSystem* log, std::string load_path);
    std::unique_ptr<Utils::Model3D> objReader(Stream::LogSystem* log, std::string load_path);

    std::optional<std::vector<char>> shaderReader(Stream::LogSystem* log, std::string file_path);
}