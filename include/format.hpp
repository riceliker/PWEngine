#pragma once

#include "math.hpp"

#include <cstdint>
#include <vector>

namespace PWEngine::Format 
{
    struct Audio
    {
        uint32_t channels;
        uint32_t sample_rate;
        uint32_t total_samples;
        uint32_t bits_per_sample;
        std::vector<uint8_t> data;
    };

    struct Image
    {
        Vec2<uint32_t> size;
        std::vector<uint8_t> data;
        uint8_t depth;
    };

    struct Vertex3D
    {    
        Vec3<float> position;
        Vec3<float> normal;
        Vec2<float> uv;
    };

    struct Model3D
    {
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;
    };
}