// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
#pragma once
#include "math.hpp"
#include "utils.hpp"
#include <cstdint>
#include <fstream>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File 
{  
    struct TGA {}; struct PNG {};
    struct ImageRGBA8
    {
        Vec2<uint32_t> size;
        std::vector<uint8_t> data;
        uint8_t depth;
        ImageRGBA8(TGA, LogSystem* log, std::string path);
        ImageRGBA8(PNG, LogSystem* log, std::string path);
    };

    struct ObjVertex3D
    {    
        Vec3<float> position;
        Vec3<float> normal;
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
            else if (line[0] == "vn")
            {
                vn.emplace_back(Vec3<float>(std::stof(line[1]), std::stof(line[2]), std::stof(line[3])));
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
                vertices.emplace_back(ObjVertex3D{v[index.x-1], vn[index.z-1], right_uv});
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