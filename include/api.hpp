#pragma once
#include "math.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace PWEngine
{
    enum class ColorFormat
    {
        RGBA8, RGBA16
    };

    struct PixelRGBA8
    {
        uint8_t r; uint8_t g; uint8_t b; uint8_t a; 
    };

    struct Vertex3D
    {    
        Vec3<float> position;
        Vec3<float> color;
        Vec2<float> uv;
    };

    struct Model3D
    {
        
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;
    };

    struct ImageRGBA8
    {
        Vec2<uint32_t> size;
        std::vector<uint8_t> data;
        uint8_t depth;
    };
}