#pragma once


#include "SDL3/SDL_pixels.h"
#include <cmath>
#include <cstdint>
namespace PWEngine::Type 
{  
    typedef uint32_t uint;
    typedef uint8_t uchar;
    struct PWEColor
    {
        public:
            uchar r; uchar g; uchar b; uchar a;
            SDL_Color forSDL()
            {
                SDL_Color color{
                    .r = (Uint8)r, .g = (Uint8)g, .b = (Uint8)b, .a = (Uint8)a
                };
                return color;
            }
            Uint32 forSDL32()
            {
                return SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888), NULL, r, g, b, a);
            }

    };
    
    template<typename T>
    class PWEVec2T
    {
        public:
            T x; T y;
            PWEVec2T<T>(T x, T y)
            {
                this->x = x;
                this->y = y;
            }
            bool operator==(const PWEVec2T<T> a) const
            {
                return (x == a.x) && (y == a.y);
            }
            bool operator!=(const PWEVec2T<T> a) const
            {
                return (x != a.x) || (y != a.y);
            }
            PWEVec2T<T> operator+(const PWEVec2T<T> a) const
            {
                return {x + a.x, y + a.y};
            }
            PWEVec2T<T> operator-(const PWEVec2T<T> a) const
            {
                return {x - a.x, y - a.y};
            }
            PWEVec2T<T> operator-() const
            {
                return {-x, -y};
            }
    };
    class PWEVec2
    {
        public:
            float x; float y;
            PWEVec2(float x_, float y_) : x(x_), y(y_) {}
            float length()
            {
                return std::sqrtf(x * x + y * y);
            }
            void normalized()
            {
                x = x / length();
                y = y / length();
            }
            void unit(float a)
            {
                x = a;
                y = a;
            }
            bool operator==(const PWEVec2 a) const
            {
                return (x == a.x) && (y == a.y);
            }
            bool operator!=(const PWEVec2 a) const
            {
                return (x != a.x) || (y != a.y);
            }
            PWEVec2 operator+(const PWEVec2 a) const
            {
                return {x + a.x, y + a.y};
            }
            PWEVec2 operator-(const PWEVec2 a) const
            {
                return {x - a.x, y - a.y};
            }
            PWEVec2 operator-() const
            {
                return {-x, -y};
            }
            PWEVec2 operator*(float lambda) const
            {
                return {lambda * x, lambda * y};
            }
            static float dot(PWEVec2 x, PWEVec2 y)
            {
                return x.x * y.x + y.x * y.y;
            }
            static float cross(PWEVec2 x, PWEVec2 y)
            {
                return x.x * y.x - y.x * y.y;
            }
            static float distance2(PWEVec2 a, PWEVec2 b)
            {
                float dx = a.x - b.x;
                float dy = a.y - b.y;
                return dx * dx + dy * dy;
            }
            static float distance(PWEVec2 a, PWEVec2 b)
            {
                return sqrt(distance2(a, b));
            }
    };
    template<typename T>
    class PWEVec3T
    {
        public:
            T x; T y; T z;
            PWEVec3T<T>(T obj, T x, T y, T z)
            {
                this->x = x;
                this->y = y;
                this->z = z;
            }
    };
    class PWEVec3
    {
        public:
            float x; float y; float z;
            PWEVec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
            float length()
            {
                return std::sqrtf(x * x + y * y + z * z);
            }
            void normalized()
            {
                x = x / length();
                y = y / length();
                z = z / length();
            }
            void unit(float a)
            {
                x = a;
                y = a;
                z = a;
            }
            bool operator==(const PWEVec3 a) const
            {
                return (x == a.x) && (y == a.y) && (z == a.z);
            }
            bool operator!=(const PWEVec3 a) const
            {
                return (x != a.x) || (y != a.y) || (z != a.z);
            }
            PWEVec3 operator+(const PWEVec3 a) const
            {
                return {x + a.x, y + a.y, z + a.z};
            }
            PWEVec3 operator-(const PWEVec3 a) const
            {
                return {x - a.x, y - a.y, z - a.z};
            }
            PWEVec3 operator-() const
            {
                return {-x, -y, -z};
            }
            PWEVec3 operator*(float lambda) const
            {
                return {lambda * x, lambda * y, lambda * z};
            }
            static float dot(PWEVec3 x, PWEVec3 y)
            {
                return x.x * y.x + x.y * y.y + x.z * y.z;
            }
            static PWEVec3 cross(PWEVec3 x, PWEVec3 y)
            {
                return {
                    x.y * y.z - x.z * y.y,
                    x.z * y.x - x.x * y.z,
                    x.x * y.y - x.y * y.x
                };
            }
            static float distance2(PWEVec3 a, PWEVec3 b)
            {
                float dx = a.x - b.x;
                float dy = a.y - b.y;
                float dz = a.z - b.z;
                return dx * dx + dy * dy + dz * dz;
            }
            static float distance(PWEVec3 a, PWEVec3 b)
            {
                return sqrt(distance2(a, b));
            }
    };
    template<typename T>
    class PWEVec4T
    {
        public:
            T x; T y; T z; T w;
            PWEVec4T<T>(T obj, T x, T y, T z, T w)
            {
                this->x = x;
                this->y = y;
                this->z = z;
                this->w = w;
            }
    };
    class PWEVec4
    {
        public:
            float x; float y; float z; float w;
            PWEVec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
            float length()
            {
                return std::sqrtf(x * x + y * y + z * z + w * w);
            }
            void normalized()
            {
                x = x / length();
                y = y / length();
                z = z / length();
                w = w / length();
            }
            void unit(float a)
            {
                x = a;
                y = a;
                z = a;
                w = a;
            }
            bool operator==(const PWEVec4 a) const
            {
                return (x == a.x) && (y == a.y) && (z == a.z) && (w == a.w);
            }
            bool operator!=(const PWEVec4 a) const
            {
                return (x != a.x) || (y != a.y) || (z != a.z) || (w != a.w);
            }
            PWEVec4 operator+(const PWEVec4 a) const
            {
                return {x + a.x, y + a.y, z + a.z, w + a.w};
            }
            PWEVec4 operator-(const PWEVec4 a) const
            {
                return {x - a.x, y - a.y, z - a.z, w - a.w};
            }
            PWEVec4 operator-() const
            {
                return {-x, -y, -z, -w};
            }
            PWEVec4 operator*(float lambda) const
            {
                return {lambda * x, lambda * y, lambda * z, lambda * w};
            }
            static float dot(PWEVec4 x, PWEVec4 y)
            {
                return x.x * y.x + x.y * y.y + x.z * y.z + x.w * y.w;
            }
            static float distance2(PWEVec4 a, PWEVec4 b)
            {
                float dx = a.x - b.x;
                float dy = a.y - b.y;
                float dz = a.z - b.z;
                float dw = a.w - b.w;
                return dx * dx + dy * dy + dz * dz + dw * dw;
            }
            static float distance(PWEVec4 a, PWEVec4 b)
            {
                return sqrt(distance2(a, b));
            }
    };
    struct PWERectSize2
    {
        uint x; uint y; uint w; uint h;
    };
    struct PWERectPos2
    {
        uint sx; uint sy; uint ex; uint ey;
    };
    [[deprecated]];
    class PWEAutoRect
    {
        private:
            PWEVec2 _start = PWEVec2(0,0);
            PWEVec2 _end = PWEVec2(0,0);
            PWEVec2 _size = PWEVec2(0,0);
        public:
            PWEVec2 start() const {return _start;}
            PWEVec2 end() const {return _end;}
            PWEVec2 size() const {return _size;}
            void start(PWEVec2 start)
            {
                _start = start;
                _end = _start + _size;
            }
            void size(PWEVec2 size)
            {
                _size = size;
                _end = _start + _size;
            }
            void end(PWEVec2 end)
            {
                _end = end;
                _size = _end - _start;
            }
            PWEAutoRect(bool sizeT_endF, float a, float b, float c, float d)
            {
                _start = PWEVec2(a, b);
                if(sizeT_endF)
                {
                    this->size(PWEVec2(c, d));
                }
                else 
                {
                    this->end(PWEVec2(c, d));
                }
            }
    };
}