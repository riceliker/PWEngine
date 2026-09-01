#pragma once

namespace PWEngine::Utils 
{
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

}