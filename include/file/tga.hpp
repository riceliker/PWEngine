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
#include "bitstream.hpp"
#include "format.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

namespace PWEngine::Tga
{ 
    enum class TgaErrorCode
    {
        Okay = 0,
        CanNotOpenFile = 1,
        NotWavFile = 2,
        NotSupportColorMap = 3,
        NotSupportGrayImage  = 4,
        NotSupportRGB5Image = 5,
        NotSupportThisPixelFormat = 6,
    };

    inline std::tuple<std::unique_ptr<Format::Image>, TgaErrorCode> TgaFile(std::string path)
    {
        auto result = std::make_unique<Format::Image>();

        auto _file = BitStream::readBinaryFile(path);
        if (!_file.has_value()) 
            return std::make_tuple(std::move(result), TgaErrorCode::CanNotOpenFile);
        
        auto file = _file.value();
        if (file.size() < 18) 
            return std::make_tuple(std::move(result), TgaErrorCode::NotWavFile);
        
        auto ptr = file.data();
        uint8_t id_length = BitStream::get8(ptr);
        uint8_t color_map_type = BitStream::get8(ptr);
        uint8_t image_type = BitStream::get8(ptr);
        uint16_t color_map_origin = BitStream::get16le(ptr);
        uint16_t color_map_length = BitStream::get16le(ptr);
        uint8_t color_max_depth = BitStream::get8(ptr);
        uint16_t x_origin = BitStream::get16le(ptr);
        uint16_t y_origin = BitStream::get16le(ptr);
        uint16_t width  = BitStream::get16le(ptr);
        uint16_t height = BitStream::get16le(ptr);
        uint8_t pixel_depth = BitStream::get8(ptr);
        uint8_t image_descriptor = BitStream::get8(ptr);

        bool start_direct = (image_descriptor & (1u << 5)) != 0;

        if (color_map_type == 1)
        {
            return std::make_tuple(std::move(result), TgaErrorCode::NotSupportColorMap);
        }
        else 
        {
            std::vector<Vec4<uint8_t>> pixels;
            switch (pixel_depth) 
            {
            case 8:
                return std::make_tuple(std::move(result), TgaErrorCode::NotSupportGrayImage);
            case 16:
                return std::make_tuple(std::move(result), TgaErrorCode::NotSupportRGB5Image);
            case 24:
                if (start_direct) {
                    for (size_t i = 0; i < width * height; ++i) {
                        uint8_t b = BitStream::get8(ptr);
                        uint8_t g = BitStream::get8(ptr);
                        uint8_t r = BitStream::get8(ptr);
                        uint8_t a = 255;
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                    }
                } else {
                    ptr += width * height - 4;
                    for (size_t i = 0; i < width * height; ++i) {
                        uint8_t b = BitStream::get8m(ptr);
                        uint8_t g = BitStream::get8m(ptr);
                        uint8_t r = BitStream::get8m(ptr);
                        uint8_t a = 255;
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                    }
                }

            break;
            case 32:
                for (size_t i = 0; i < width * height; ++i) {
                    uint8_t b = BitStream::get8(ptr);
                    uint8_t g = BitStream::get8(ptr);
                    uint8_t r = BitStream::get8(ptr);
                    uint8_t a = BitStream::get8(ptr);
                    pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                }
                if (!start_direct) {
                    for(size_t y = 0; y < height / 2; y++) {
                        size_t y2 = height - 1 - y;
                        for(size_t x = 0; x < width; x++) {
                            std::swap(
                                pixels[y * width + x],
                                pixels[y2 * width + x]
                            );
                        }
                    }
                }
            break;
            default:
                return std::make_tuple(std::move(result), TgaErrorCode::NotSupportThisPixelFormat);
            }

            result->data.resize(pixels.size() * sizeof(Vec4<uint8_t>));
            memcpy(result->data.data(), pixels.data(), result->data.size());

            result->size = Vec2<uint32_t>(width, height);
            result->depth = pixel_depth;

            return std::make_tuple(std::move(result), TgaErrorCode::Okay);
        }   
    }
}