#pragma once
#include "api.hpp"
#include "stream.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File 
{  
    std::unique_ptr<ImageRGBA8> tgaReader(Stream::LogSystem* log, std::string load_path);
    std::unique_ptr<Model3D> objReader(Stream::LogSystem* log, std::string load_path);

    std::optional<std::vector<char>> shaderReader(Stream::LogSystem* log, std::string file_path);
}