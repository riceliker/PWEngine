#include "stream.hpp"
#include <optional>
#include <vector>
#include <fstream>

namespace PWEngine::File 
{
    std::optional<std::vector<char>> shaderReader(Stream::LogSystem* log, std::string file_path)
    {
        std::ifstream file(file_path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) 
        {
            Stream::log(log, Stream::LogType::Warn, Stream::LogFrom::FileIO, "failed to open file!");
            return std::nullopt;
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
    
}