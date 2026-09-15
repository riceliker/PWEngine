#pragma once
#include <cmath>
#include <cstddef>
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

    constexpr float deg2rad(float deg) noexcept
    {
        constexpr float pi = 3.14159265358979323846;
        return deg * pi / 180;
    }

    constexpr float rad2deg(float rad) noexcept
    {
        constexpr float pi = 3.14159265358979323846;
        return rad * 180 / pi;
    }

    template<typename T>
    struct Vec2
    {
        T x; T y;
        Vec2(T x, T y):x(x), y(y){}
        Vec2(){}
    };

    template<typename T>
    struct Vec3
    {
        T x; T y; T z;
        Vec3(T x, T y, T z):x(x), y(y), z(z){}
        Vec3(){};
    };

    inline constexpr Vec3<float> operator-(Vec3<float> a, Vec3<float> b) noexcept
    {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    inline constexpr Vec3<float> cross(Vec3<float> a, Vec3<float> b) noexcept
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    inline constexpr float dot(Vec3<float> a, Vec3<float> b) noexcept
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    inline constexpr Vec3<float> normal(Vec3<float> v) noexcept
    {
        float len = sqrt(dot(v, v));
        return { v.x / len, v.y / len, v.z / len };
    }

    template<typename T>
    struct Vec4
    {
        T x; T y; T z; T w;
        Vec4(T x, T y, T z, T w): x(x), y(y), z(z), w(w){}
        Vec4(){};
    };

    struct Mat4
    {
        float data[16];
        constexpr float& rc(size_t row, size_t col) noexcept {return data[col * 4 + row];}
        constexpr const float& rc(size_t row, size_t col) const noexcept {return data[col * 4 + row];}
        constexpr float& cr(size_t col, size_t row) noexcept {return data[col * 4 + row];}
        constexpr const float& cr(size_t col, size_t row) const noexcept {return data[col * 4 + row];}
        Mat4(float a){for(auto& f : data) f = 0.0f;rc(0,0)=a; rc(1,1)=a; rc(2,2)=a; rc(3,3)=a;}
    };

    inline constexpr Mat4 operator*(const Mat4& A, const Mat4& B) noexcept
    {
        Mat4 out = Mat4(1);
        for (size_t r = 0; r < 4; r++)
        {
            for (size_t c = 0; c < 4; c++)
            {
                float sum = 0;
                for (size_t k = 0; k < 4; k++)
                {
                    sum += A.rc(r, k) * B.rc(k, c);
                }
                out.rc(r, c) = sum;
            }
        }
        return out;
    }

    constexpr Mat4 translate(Vec3<float> v) noexcept
    {
        Mat4 m = Mat4(1);
        m.rc(0,3) = v.x;
        m.rc(1,3) = v.y;
        m.rc(2,3) = v.z;
        return m;
    }

    constexpr Mat4 scale(Vec3<float> v) noexcept
    {
        Mat4 m = Mat4(1);
        m.rc(0,0) = v.x;
        m.rc(1,1) = v.y;
        m.rc(2,2) = v.z;
        return m;
    }

    inline constexpr Mat4 rotate(float rad, Vec3<float> axis) noexcept
    {
        Mat4 rot = Mat4(1);
        float x = axis.x; float y = axis.y; float z = axis.z;
        float c = cos(rad); float s = sin(rad); float omc = 1.0 - c;
        rot.rc(0,0) = c + omc * x*x; rot.rc(0,1) = omc * x*y - s * z; rot.rc(0,2) = omc * x*z + s * y;
        rot.rc(1,0) = omc * y*x + s * z; rot.rc(1,1) = c + omc * y*y; rot.rc(1,2) = omc * y*z - s * x;
        rot.rc(2,0) = omc * z*x - s * y; rot.rc(2,1) = omc * z*y + s * x; rot.rc(2,2) = c + omc * z*z;
        return rot;
    }

    inline constexpr Mat4 rotate(Mat4 left, float rad, Vec3<float> axis)
    {
        auto mat4 = rotate(rad, axis);
        return left * mat4;
    }

    inline constexpr Mat4 look(Vec3<float> view, Vec3<float> center, Vec3<float> world) noexcept
    {
        Vec3<float> f = normal(center - view);
        Vec3<float> r = cross(f, world);

        float lenSq = dot(r, r);
        if (lenSq < 1e-8f)
        {
            r = cross(f, Vec3<float>{1.0f, 0.0f, 0.0f});
            lenSq = dot(r, r);
            if (lenSq < 1e-8f)
                r = cross(f, Vec3<float>{0.0f, 1.0f, 0.0f});
        }

        Vec3<float> u = cross(r, f);

        Mat4 R = Mat4(1);
        R.rc(0,0) = r.x; R.rc(0,1) = u.x; R.rc(0,2) = f.x;
        R.rc(1,0) = r.y; R.rc(1,1) = u.y; R.rc(1,2) = f.y;
        R.rc(2,0) = r.z; R.rc(2,1) = u.z; R.rc(2,2) = f.z;

        Mat4 T = translate(Vec3<float>(-view.x, -view.y, -view.z));
        return R * T;
    }

    constexpr Mat4 perspective(float fovY_rad, float aspect, float near, float far) noexcept
    {
        Mat4 m = Mat4(1.0f);

        float tanHalfFov = tan(fovY_rad / 2);

        m.rc(0,0) = 1 / (tanHalfFov * aspect);
        m.rc(1,1) = 1 / tanHalfFov;
        m.rc(2,2) = far / (far - near);
        m.rc(2,3) = -(far * near) / (far - near);
        m.rc(3,2) = 1;
        m.rc(3,3) = 0;
        return m;
    }

    inline float str2float(const char* str)
    {
        const char* w = str;
        while(*w) ++w;
        const char* begin = str;
        const char* end = w;

        if (begin >= end) return 0.0f;

        const char* p = begin;
        bool negative = false;
        if (*p == '-')
        {
            negative = true;
            p++;
        }
        else if (*p == '+')
        {
            p++;
        }

        float result = 0.0f;
        // XX.
        while (p < end && *p >= '0' && *p <= '9')
        {
            result = result * 10.0f + (*p - '0');
            p++;
        }

        // .XX
        if (p < end && *p == '.')
        {
            p++;
            float frac = 0.1f;
            while (p < end && *p >= '0' && *p <= '9')
            {
                result += (*p - '0') * frac;
                frac *= 0.1f;
                p++;
            }
        }

        // XXeXX
        if (p < end && (*p == 'e' || *p == 'E'))
        {
            p++;
            int exp_neg = 0;
            int exponent = 0;
            if (p < end && *p == '-')
            {
                exp_neg = 1;
                p++;
            }
            else if (p < end && *p == '+')
            {
                p++;
            }

            while (p < end && *p >= '0' && *p <= '9')
            {
                exponent = exponent * 10 + (*p - '0');
                p++;
            }
            if (exp_neg) exponent = -exponent;

            result *= powf(10.0f, static_cast<float>(exponent));
        }

        if (negative)
            result = -result;

        return result;
    }


    struct Vertex3D
    {    
        Utils::Vec3<float> position;
        Utils::Vec3<float> color;
        Utils::Vec2<float> uv;
    };

    struct ImageRGBA8
    {
        Utils::Vec2<uint32_t> size;
        std::vector<uint8_t> data;
        uint8_t depth;
    };

}