#pragma once

#include "utils.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include <fstream>

namespace PWEngine::File 
{ 
    inline std::optional<std::vector<uint8_t>> readBinaryFile(LogSystem* log, std::string path)
    {
        sendLog(log, LogType::Info, LogFrom::FileIO, "Open file:" + path);
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            sendLog(log, LogType::Info, LogFrom::FileIO, "Can not open the file.");
            return std::nullopt;
        }

        std::streamsize file_bytes = file.tellg();

        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> buffer(static_cast<size_t>(file_bytes));

        if (!file.read(reinterpret_cast<char*>(buffer.data()), file_bytes))
        {
            sendLog(log, LogType::Info, LogFrom::FileIO, "Can not read the file.");
            return std::nullopt;
        }
        return buffer;
    }

    inline uint8_t get8(uint8_t*& ptr)
    {
        return *ptr++;
    }

    inline uint8_t get8m(uint8_t*& ptr)
    {
        return *ptr--;
    }

    inline uint16_t get16le(uint8_t*& ptr)
    {
        uint16_t out = 0;
        out |= *(ptr++) << (8 * 0);
        out |= *(ptr++) << (8 * 1);
        return out;
    }

    inline uint32_t get32le(uint8_t*& ptr)
    {
        uint32_t out = 0;
        out |= *(ptr++) << (8 * 0);
        out |= *(ptr++) << (8 * 1);
        out |= *(ptr++) << (8 * 2);
        out |= *(ptr++) << (8 * 3);
        return out;
    }

    inline uint16_t get16be(uint8_t*& ptr)
    {
        uint16_t out = 0;
        out |= *(ptr++) << (8 * 1);
        out |= *(ptr++) << (8 * 0);
        return out;
    }

    inline uint32_t get32be(uint8_t*& ptr)
    {
        uint32_t out = 0;
        out |= *(ptr++) << (8 * 3);
        out |= *(ptr++) << (8 * 2);
        out |= *(ptr++) << (8 * 1);
        out |= *(ptr++) << (8 * 0);
        return out;
    }
}