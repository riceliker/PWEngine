#include "file.hpp"
#include "stream.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File 
{
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

    std::unique_ptr<Model3D> objReader(Stream::LogSystem* log, std::string load_path)
    {
        std::fstream file(load_path);
        if (!file.is_open())
        {
            Stream::log(log, Stream::LogType::Warn, Stream::LogFrom::FileIO, "Can no open the "+ load_path +" file");
        }
        std::string text_line;
        std::vector<Vertex3D> vertices;
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
                vertices.emplace_back(Vertex3D{v[index.x-1], {1, 1, 1}, right_uv});
            }
        }
        for (size_t i = 0; i <= f.size()*3; ++i)
        {
            indics.push_back(i);
        }
        auto obj = std::make_unique<Model3D>();
        obj->vertices = std::move(vertices);
        obj->indices = std::move(indics);
        return obj;
    }
}