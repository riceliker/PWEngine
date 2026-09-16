#pragma once
#include "stream.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File 
{  
    class TgaStream
    {
    private:
        std::vector<uint8_t> binary;
        std::vector<Utils::PixelRGBA8> pixels;
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
        FileInfo info;
    public:
        TgaStream(Stream::LogSystem* log, std::string load_path);
        std::unique_ptr<Utils::ImageRGBA8> makeImage();
        std::unique_ptr<Utils::ImageRGBA8> asImage();
    };
    
    class ObjStream
    {
    private:
        std::vector<Utils::Model3D::Vertex3D> vertices;
        std::vector<uint32_t> indics;

        std::vector<Utils::Vec3<float>> v;
        std::vector<Utils::Vec2<float>> vt;
        std::vector<Utils::Vec3<float>> vn;
        std::vector<std::vector<Utils::Vec3<uint32_t>>> f;
    public:
        ObjStream(Stream::LogSystem*, std::string load_path);
        std::vector<Utils::Model3D::Vertex3D> asVertex3D();
        std::vector<uint32_t> asIndices();
        std::unique_ptr<Utils::Model3D> asModel();
    };
}