#pragma once
#include <cstdint>
#include <vector>

namespace PWEngine::Utils 
{
    enum class ColorFormat
    {
        RGBA8, RGBA16
    };

    struct PixelRGBA8
    {
        uint8_t r; uint8_t g; uint8_t b; uint8_t a; 
    };

    struct PixelRGB8
    {
        uint8_t r; uint8_t g; uint8_t b;
    };


    template<typename T>
    struct Vec2
    {
        T x; T y;
        Vec2(T x, T y):x(x), y(y){}
        Vec2(){};
    };

    template<typename T>
    struct Vec3
    {
        T x; T y; T z;
        Vec3(T x, T y, T z):x(x), y(y), z(z){}
        Vec3(){};
    };

    template<typename T>
    struct Vec4
    {
        T x; T y; T z; T w;
        Vec4(T x, T y, T z, T w): x(x), y(y), z(z), w(w){}
        Vec4(){};
    };

    struct Image
    {
        Utils::Vec2<size_t> size;
        std::vector<Utils::PixelRGBA8> data;
        uint8_t depth;
    };

}