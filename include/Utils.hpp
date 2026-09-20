#pragma once
#include <algorithm>
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
    inline constexpr Vec2<T> operator+(Vec2<T> a, Vec2<T> b) noexcept
    {
        return {a.x + b.x, a.y + b.y};
    }

    template<typename T>
    inline constexpr Vec2<T> operator-(Vec2<T> a, Vec2<T> b) noexcept
    {
        return {a.x - b.x, a.y - b.y};
    }

    template<typename T>
    struct Vec3
    {
        T x; T y; T z;
        Vec3(T x, T y, T z):x(x), y(y), z(z){}
        Vec3(){};
    };

    inline constexpr Vec3<float> operator+(Vec3<float> a, Vec3<float> b) noexcept
    {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    inline constexpr Vec3<float> operator-(Vec3<float> a, Vec3<float> b) noexcept
    {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }


    inline constexpr Vec3<float> operator*(float a, Vec3<float> b) noexcept
    {
        return {a * b.x, a * b.y, a * b.z};
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

    inline constexpr Vec4<float> euler2quat(Vec3<float> euler) noexcept
    {
        float cx = std::cos(euler.x * 0.5f);
        float sx = std::sin(euler.x * 0.5f);
        float cy = std::cos(euler.y * 0.5f);
        float sy = std::sin(euler.y * 0.5f);
        float cz = std::cos(euler.z * 0.5f);
        float sz = std::sin(euler.z * 0.5f);

        return {
            sx*cy*cz - cx*sy*sz,
            -cx*sy*cz - sx*cy*sz,
            cx*cy*sz - sx*sy*cz,
            cx*cy*cz + sx*sy*sz,
        };
    };  

    inline constexpr Vec3<float> quat2euler(const Vec4<float> quat) noexcept
    {
        Vec3<float> e{};
        float x = quat.x;
        float y = quat.y;
        float z = quat.z;
        float w = quat.w;

        // sin(pitch)
        float sin_pitch = 2.0f * ( w*y - x*z );
        sin_pitch = std::clamp(sin_pitch, -1.0f, 1.0f);
        e.y = std::asin(sin_pitch); // pitch -> Y

        float cp = std::cos(e.y);
        if (std::fabs(cp) > 1e-6f)
        {
            e.x = std::atan2( 2*(w*x + y*z), 1 - 2*(x*x + y*y) ); // yaw -> X
            e.z = std::atan2( 2*(w*z + x*y), 1 - 2*(y*y + z*z) ); // roll -> Z
        }
        else
        {
            // pitch -> ±90°
            e.x = 0.0f;
            e.z = std::atan2(2*(x*y + w*z), 1 - 2*(y*y + z*z));
        }
        return e;
    }

    inline constexpr Vec4<float> quat_mul(const Vec4<float>& a, const Vec4<float>& b) noexcept
    {
        Vec4<float> r;
        r.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
        r.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
        r.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
        r.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
        return r;
    }

    inline constexpr Vec4<float> quat_normalize(Vec4<float> q) noexcept
    {
        float len_sq = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
        float len = std::sqrt(len_sq);
        if(len < 1e-6f)
            return {0,0,0,1};
        q.x /= len;
        q.y /= len;
        q.z /= len;
        q.w /= len;
        return q;
    }

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
    
    inline constexpr Mat4 transform(Vec3<float> p, Vec3<float> s, const Vec4<float> r) noexcept
    {
        Mat4 mat = Mat4(1);
        /* rotate */
        float xx = 2.0f * r.x * r.x; float xy = 2.0f * r.x * r.y; float wx = 2.0f * r.w * r.x;
        float yy = 2.0f * r.y * r.y; float xz = 2.0f * r.x * r.z; float wy = 2.0f * r.w * r.y;
        float zz = 2.0f * r.z * r.z; float yz = 2.0f * r.y * r.z; float wz = 2.0f * r.w * r.z;
        mat.rc(0,0) = 1.0f - yy - zz; mat.rc(1,0) = xy + wz; mat.rc(2,0) = xz - wy;
        mat.rc(0,1) = xy - wz; mat.rc(1,1) = 1.0f - xx - zz; mat.rc(2,1) = yz + wx;
        mat.rc(0,2) = xz + wy; mat.rc(1,2) = yz - wx; mat.rc(2,2) = 1.0f - xx - yy;  
        /* scale */
        mat.rc(0,0) *= s.x; mat.rc(0,1) *= s.y; mat.rc(0,2) *= s.z;
        mat.rc(1,0) *= s.x; mat.rc(1,1) *= s.y; mat.rc(1,2) *= s.z;
        mat.rc(2,0) *= s.x; mat.rc(2,1) *= s.y; mat.rc(2,2) *= s.z;
        /* postion */
        mat.rc(3,0) = p.x;
        mat.rc(3,1) = p.y;
        mat.rc(3,2) = p.z;
        return mat;
    }

    inline constexpr Mat4 look(Vec3<float> view, Vec3<float> center, Vec3<float> world) noexcept
    {
        Vec3<float> f = normal(center - view);
        Vec3<float> fh = Vec3<float>(f.x , f.y , 0);
        Vec3<float> r = cross(fh, world);

        float lenSq = dot(r, r);
        if (lenSq < 1e-8f)
        {
            r = cross(f, Vec3<float>{1.0f, 0.0f, 0.0f});
            lenSq = dot(r, r);
            if (lenSq < 1e-8f)
                r = cross(f, Vec3<float>{0.0f, 1.0f, 0.0f});
        }
        r = normal(r);

        Vec3<float> u = cross(f, r);
        u = Vec3<float>(-u.x, -u.y, -u.z);

        Mat4 R = Mat4(1);
        R.rc(0,0) = r.x; R.rc(0,1) = r.y; R.rc(0,2) = r.z;
        R.rc(1,0) = u.x; R.rc(1,1) = u.y; R.rc(1,2) = u.z;
        R.rc(2,0) = f.x; R.rc(2,1) = f.y; R.rc(2,2) = f.z;

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

    struct Vertex3D
    {    
        Utils::Vec3<float> position;
        Utils::Vec3<float> color;
        Utils::Vec2<float> uv;
    };

    struct Model3D
    {
        
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;
    };

    struct ImageRGBA8
    {
        Utils::Vec2<uint32_t> size;
        std::vector<uint8_t> data;
        uint8_t depth;
    };

}