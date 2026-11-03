// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
#pragma once
#include "math.hpp"
#include "stream.hpp"
#include <cstdint>
#include <fstream>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File 
{  
    static inline std::optional<std::vector<uint8_t>> readBinaryFile(LogSystem* log, std::string path)
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

    static inline uint8_t su8(std::vector<uint8_t> ptr, size_t offset)
    {
        uint8_t out = 0;
        out |= ptr[offset];
        return out;
    }

    static inline uint16_t su16(std::vector<uint8_t> ptr, size_t offset)
    {
        uint16_t out = 0;
        for (int i = 0; i < 2; i++)
        {
            out |= ptr[offset + i] << (8 * i);
        }
        return out;
    }

    static inline uint16_t su32(std::vector<uint8_t> ptr, size_t offset)
    {
        uint32_t out = 0;
        for (int i = 0; i < 4; i++)
        {
            out |= ptr[offset + i] << (8 * i);
        }
        return out;
    }

    static inline uint16_t bu16(std::vector<uint8_t> ptr, size_t offset)
    {
        uint32_t out = 0;
        for (int i = 0; i < 2; i++)
        {
            out |= ptr[offset + i] << (8 * (1 - i));
        }
        return out;
    }

    static inline uint16_t bu32(std::vector<uint8_t> ptr, size_t offset)
    {
        uint32_t out = 0;
        for (int i = 0; i < 4; i++)
        {
            out |= ptr[offset + i] << (8 * (3 - i));
        }
        return out;
    }

    struct ImageRGBA8
    {
        Vec2<uint32_t> size;
        std::vector<uint8_t> data;
        uint8_t depth;
    };

    struct ObjVertex3D
    {    
        Vec3<float> position;
        Vec3<float> color;
        Vec2<float> uv;
    };

    struct ObjModel3D
    {
        std::vector<ObjVertex3D> vertices;
        std::vector<uint32_t> indices;
    };

    static inline std::vector<std::string> spiltBy(std::string_view str, char sym)
    {
        std::vector<std::string> out;
        std::string token = "";
        for (const auto& word: str)
        {
            if (word == sym)
            {
                out.push_back(token);
                token = "";
            }
            else
                token += word; 
        }
        out.push_back(token);
        return out;
    }

    struct FileInfo
    {
        uint8_t ID_length;
        uint8_t color_map_type;
        uint8_t image_type;
        uint16_t color_map_origin;
        uint16_t color_map_length;
        uint8_t color_max_depth;
        uint16_t x_origin;
        uint16_t y_origin;
        uint16_t width;
        uint16_t height;
        uint8_t pixel_depth;
        uint8_t image_descriptor;
    };

    inline std::unique_ptr<ImageRGBA8> tgaReader(LogSystem* log, std::string load_path)
    {
        auto file_maybe = readBinaryFile(log, load_path);
        size_t offset = 0;
        uint16_t width;
        uint16_t height;
        if (file_maybe.has_value())
        {
            auto file = file_maybe.value();
            FileInfo info;
            std::vector<Vec4<uint8_t>> pixels;
            info.ID_length = file[0];
            info.color_map_type = file[1];
            info.image_type = file[2];
            info.color_map_origin = su16(file, 3);
            info.color_map_length = su16(file, 5);
            info.color_max_depth = file[7];
            info.x_origin = su16(file, 8);
            info.y_origin = su16(file, 10);
            width = su16(file, 12);
            info.width = width;
            height = su16(file, 14);
            info.height = height;
            info.pixel_depth = file[16];
            info.image_descriptor = file[17];

            offset += 18;
            // have color map? do.
            if (info.color_map_type == 1)
            {
                uint8_t entry_size = (info.color_max_depth + 7) / 8;
                offset += (size_t)info.color_map_length * entry_size;
            }
            if (info.pixel_depth == 24)
            {
                for (size_t i = 0; i < width * height; ++i)
                {
                    uint8_t b = file[offset];
                    uint8_t g = file[offset + 1];
                    uint8_t r = file[offset + 2];
                    uint8_t a = 255;
                    pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                    offset += 3;
                }
            }
            else if (info.pixel_depth == 32)
            {
                if ((info.image_descriptor & (1u << 5)) != 0)
                {
                    for (size_t i = 0; i < width * height; ++i)
                    {
                        uint8_t b = file[offset];
                        uint8_t g = file[offset + 1];
                        uint8_t r = file[offset + 2];
                        uint8_t a = file[offset + 3];
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                        offset += 4;
                    }
                }
                else 
                {
                    offset += width * height - 4;
                    for (size_t i = 0; i < width * height; ++i)
                    {
                        uint8_t b = file[offset];
                        uint8_t g = file[offset + 1];
                        uint8_t r = file[offset + 2];
                        uint8_t a = file[offset + 3];
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                        offset -= 4;
                    }
                }
                
            }
            else 
            {
                sendLog(log, LogType::Warn, LogFrom::FileIO ,"The pixel format is not support.");
            }
            std::unique_ptr<ImageRGBA8> obj = std::make_unique<ImageRGBA8>();
            std::vector<uint8_t> data;
            data.resize(pixels.size() * sizeof(Vec4<uint8_t>));
            memcpy(data.data(), pixels.data(), data.size());

            if ((info.image_descriptor & (1u << 5)) != 0)
            {

            }
            obj->data = std::move(data);
            obj->size = Vec2<uint32_t>(info.width, info.height);
            obj->depth = info.pixel_depth;
            
            return obj;
        } 
        return nullptr;
    }

    inline std::unique_ptr<ObjModel3D> objReader(LogSystem* log, std::string load_path)
    {
        std::fstream file(load_path);
        if (!file.is_open())
        {
            sendLog(log, LogType::Warn, LogFrom::FileIO, "Can no open the "+ load_path +" file");
        }
        std::string text_line;
        std::vector<ObjVertex3D> vertices;
        std::vector<uint32_t> indics;

        std::vector<Vec3<float>> v;
        std::vector<Vec2<float>> vt;
        std::vector<Vec3<float>> vn;
        std::vector<std::vector<Vec3<uint32_t>>> f;
        while(std::getline(file, text_line))
        {
            auto line = spiltBy(text_line, ' ');
            if (line[0] == "")
            {
                continue;
            }
            else if (line[0] == "v")
            {
                v.emplace_back(Vec3<float>(std::stof(line[1]), std::stof(line[2]), std::stof(line[3])));
            }
            else if (line[0] == "vt")
            {
                vt.emplace_back(Vec2<float>(std::stof(line[1]), std::stof(line[2])));
            }
            else if (line[0] == "f")
            {
                std::vector<Vec3<uint32_t>> face;
                for (int i = 1; i < line.size(); ++i)
                {
                    auto face_index = spiltBy(line[i], '/');
                    face.emplace_back(Vec3<uint32_t>(std::stoi(face_index[0]), std::stoi(face_index[1]), std::stoi(face_index[2])));
                }
                f.push_back(std::move(face));
            }
            else 
            {
                continue;
            }
        }

        for (size_t i = 0; i < f.size(); ++i)
        {
            for (const auto& index : f[i])
            {
                auto right_uv = vt[index.y-1];
                right_uv.y = 1 - right_uv.y;
                vertices.emplace_back(ObjVertex3D{v[index.x-1], {1, 1, 1}, right_uv});
            }
        }
        for (size_t i = 0; i <= f.size()*3; ++i)
        {
            indics.push_back(i);
        }
        auto obj = std::make_unique<ObjModel3D>();
        obj->vertices = std::move(vertices);
        obj->indices = std::move(indics);
        return obj;
    }
}