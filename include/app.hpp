// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#include "gpu.hpp"
#include "math.hpp"
#include <memory>
#include <string>
namespace PWEngine::App 
{
     
    class Image
    {
    private:
        std::unique_ptr<GPU::GPUImage> image;
    public:
        Image(std::string path);
        Image(Vec4<uint8_t> color_rgba8);
    };

    struct RenderRect
    {
        Vec2<float> position;
        Vec2<float> size;
        Image* image;
    };

    class Render : public PWEngine::GPU::GPUTaskWithWindow
    {
    private:
        std::unique_ptr<PWEngine::GPU::Pipeline> world_pipeline;
        std::unique_ptr<PWEngine::GPU::Pipeline> canvas_pipeline;
    public:
        bool loop(double delta) override;
    };

    class App
    {
    private:
        std::unique_ptr<GPU::GPUInstance> instance;
    public:
        virtual bool loop(double delta) = 0;
        App(); 
        void run();
    };
}