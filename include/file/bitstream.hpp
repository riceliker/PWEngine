// SPDX-License-Identifier: MIT
// Copyright (C) 2026 riceliker
/*
MIT License 

Copyright (c) 2026 riceliker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <fstream>
#include <array>

namespace PWEngine::BitStream 
{
    inline std::optional<std::vector<uint8_t>> readBinaryFile(std::string path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            return std::nullopt;
        }

        std::streamsize file_bytes = file.tellg();

        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> buffer(static_cast<size_t>(file_bytes));

        if (!file.read(reinterpret_cast<char*>(buffer.data()), file_bytes))
        {
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

    inline bool isBlock(std::array<char, 4> text, uint32_t input)
    {
        uint32_t out = 0;
        out |= text[0] << (8 * 0);
        out |= text[1] << (8 * 1);
        out |= text[2] << (8 * 2);
        out |= text[3] << (8 * 3);

        return input == out;
    }
}