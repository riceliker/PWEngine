// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
#pragma once

#include "math.hpp"
#include "gpu.hpp"

namespace PWEngine::Render
{
    struct RectTexture
    {

    };

    class RectPipeline
    {
    private:
        
    public:
        std::unique_ptr<GPU::Pipeline> p_pipeline;
        std::unique_ptr<GPU::BindingLayout> set_texture; /* set = 0 */
        RectPipeline(GPU::GPUInstance* instance);
    };


    struct UnlitVertex
    {
        Vec3<float> position;
        Vec2<float> uv;
    };

    class UnlitMesh
    {
    private:
        GPU::GPUVertex* vertex;
    };

    class UnlitTexture
    {

    };

    class UnlitCamera
    {

    };

    class UnlitPipeline : public GPU::Pipeline
    {
    public:
        
    };
}