#pragma once
#include "stream.hpp"
#include <cstddef>
#include <fstream>
#include <cstdint>
#include <vector>
#include <optional>

namespace PWEngine::File 
{
    inline std::optional<std::vector<uint8_t>> readBinaryFile(Stream::LogSystem* log, std::string path)
    {
        Stream::log(log, Stream::LogType::Info, Stream::LogFrom::FileIO, "Open file:" + path);
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            Stream::log(log, Stream::LogType::Info, Stream::LogFrom::FileIO, "Can not open the file.");
            return std::nullopt;
        }

        std::streamsize file_bytes = file.tellg();

        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> buffer(static_cast<size_t>(file_bytes));

        if (!file.read(reinterpret_cast<char*>(buffer.data()), file_bytes))
        {
            Stream::log(log, Stream::LogType::Info, Stream::LogFrom::FileIO, "Can not read the file.");
            return std::nullopt;
        }
        Stream::log(log, Stream::LogType::Info, Stream::LogFrom::FileIO, "Read file:" + path);
        return buffer;
    }

    inline uint16_t su16(std::vector<uint8_t> ptr, size_t offset)
    {
        uint32_t out = 0;
        for (int i = 0; i < 2; i++)
        {
            out |= ptr[offset + i] << (8 * i);
        }
        return out;
    }

    inline uint16_t su32(std::vector<uint8_t> ptr, size_t offset)
    {
        uint32_t out = 0;
        for (int i = 0; i < 4; i++)
        {
            out |= ptr[offset + i] << (8 * i);
        }
        return out;
    }

    inline uint16_t bu16(std::vector<uint8_t> ptr, size_t offset)
    {
        uint32_t out = 0;
        for (int i = 0; i < 2; i++)
        {
            out |= ptr[offset + i] << (8 * (1 - i));
        }
        return out;
    }

    inline uint16_t bu32(std::vector<uint8_t> ptr, size_t offset)
    {
        uint32_t out = 0;
        for (int i = 0; i < 4; i++)
        {
            out |= ptr[offset + i] << (8 * (3 - i));
        }
        return out;
    }

    
}
