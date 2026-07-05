/**
 * @file PWLibrary.h
 * @author riceliker and all constructors (github: https://github.com/riceliker)
 * @brief 
 * @version 0.1.0 beta
 * @date 2026-07-05
 * 
 * @copyright Copyright (c) 2026 by riceliker and constructors. Open source license: LGPL3.0
 * 
 * Notice: It is a private file in library. Do not use it in your project, or you should include many libraries in your project.
 * This file is C header file. Not C++ header file. Do NOT use C++ features in this file.
 */
#pragma once
#include "PWLibrary.h"
// SDL Library
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

struct PWLibrary_Engine
{
    SDL_GPUDevice* device;
};

struct PWLibrary_WindowInfo
{
        char* name;
        int physic_w;
        int physic_h;
        int logical_w;
        int logical_h;
        PWLibrary_WindowMode mode;
};

struct PWLibrary_Window
{
        char* name;
        SDL_Window* window;
        SDL_Renderer* render;
};

struct PWLibrary_Font
{
       TTF_Font* font; 
};

struct PWLibrary_Surface
{
        SDL_Surface* surface;
};