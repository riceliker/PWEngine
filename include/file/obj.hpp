// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
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
#include "math.hpp"
#include "format.hpp"

#include <cstdint>
#include <fstream>
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace PWEngine::Obj
{ 
    static inline std::vector<std::string> spiltBy(std::string_view str, char sym)
    {
        std::vector<std::string> out;
        std::string token = "";
        for (const auto& word: str)
        {
            if (word == sym)
            {
                if (token.size() == 0) continue;
                out.push_back(token);
                token = "";
            }
            else
                token += word; 
        }
        out.push_back(token);
        return out;
    }

    enum class ObjErrCode 
    {
        Okay = 0,
        CanNotOpenFile = 1,
    };

    inline std::tuple<std::unique_ptr<Format::Model3D>, ObjErrCode> ObjFile(std::string path)
    {
        auto result = std::make_unique<Format::Model3D>();

        std::fstream file(path);
        if (!file.is_open())
            return std::make_tuple(std::move(result), ObjErrCode::CanNotOpenFile);
    
        std::string text_line;
        std::vector<Format::Vertex3D> vertices;
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
                vertices.emplace_back(Format::Vertex3D{v[index.x-1], vn[index.z-1], right_uv});
            }
        }
        for (size_t i = 0; i <= f.size()*3; ++i)
        {
            indics.push_back(i);
        }

        result->vertices = std::move(vertices);
        result->indices = std::move(indics);
        return std::make_tuple(std::move(result), ObjErrCode::Okay);
    }
}