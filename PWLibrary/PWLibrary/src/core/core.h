/**
 * @file PWL.h
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
#include "PWL.h"
#include "collections/khash.h"
#include "collections/kvec.h"
// SDL Library
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

/// ===== Engine Module ===== ///
struct PWL_Engine
{
    SDL_GPUDevice* device;
};

/// ===== Window Module ===== ///
struct PWL_WindowInfo
{
        char* name;
        int physic_w;
        int physic_h;
        int logical_w;
        int logical_h;
        PWL_WindowMode mode;
};


struct PWL_Window
{
        char* name;
        SDL_Window* window;
        SDL_Renderer* render;
};

/// ===== Font Module ===== ///
struct PWL_Font
{
       TTF_Font* font; 
};

/// ===== Surface Module ===== ///
KHASH_MAP_INIT_STR(PWL_SurfacePool_HasMap, SDL_Surface*)

struct PWL_Surface
{
        SDL_Surface* surface;
};

struct PWL_SurfacePool
{
        khash_t(PWL_SurfacePool_HasMap)* surfaces_list;
};

SDL_Surface* PWL_GetSurfaceInSurfacePool(PWL_SurfacePool* pool, char* name);
void PWL_SetSurfaceInSurfacePool(PWL_SurfacePool* pool, char* name, SDL_Surface* surface);

/// ===== Scene Module ===== ///
struct PWL_Texture
{
        SDL_Texture* texture;
        bool is_visible;
};

struct PWL_SceneLoopPackage
{

};

struct PWL_Scene
{
        SDL_Renderer* not_delete_render;
        char* (*Loop)(char* next_scene, float delta, PWL_SceneLoopPackage* self);
        kvec_t(PWL_Texture*)* texture_levels;
};

void PWL_AddSceneToWindow(PWL_Window* window, char* scene_name, PWL_Scene* scene);