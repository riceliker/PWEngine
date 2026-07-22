#pragma once

#include <memory>
#include <optional>
#include <string>

namespace PWL::Core
{
    template<typename T>
    class Vec2
    {
    public:
        T x;
        T y;
        Vec2<T>(T x, T y)
        {
            this->x = x;
            this->y = y;
        }
    };

    class PWL_Color
    {
    public:
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    class Image
    {
    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        Image(void* surface);
    };

    class ImageGroup
    {
    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        ImageGroup(std::string folder_path);
        ~ImageGroup();
        void Load();
        void Sleep();
        std::optional<std::shared_ptr<Image>> GetImage(std::string name);
    };


    enum ApplicationWindowMode
    {
        PWL_WINDOWMODE_ERROR=-1,
        PWL_WINDOWMODE_FULLSCREEN=0,
        PWL_WINDOWMODE_RESIZABLE=1
    };
    
    class Application
    {
    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    public:
        ~Application();
        static std::optional<std::shared_ptr<Application>> CreateApplication(std::string name, Vec2<uint32_t> physics_resolution, Vec2<uint32_t> logical_resolution, ApplicationWindowMode mode);
    };
}