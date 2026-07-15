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
#include "SDL3/SDL_stdinc.h"
#include "collections/klib/khash.h"
#include "collections/klib/kstring.h"
#include "collections/klib/kvec.h"
// SDL Library
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

/// ===== KLib Utils ===== ///
static void str_k(const char* src, kstring_t* out)
{
    out->l = 0;
    if (src == NULL || *src == '\0')
        return;
    kputs(src, out);
}

/// ===== Engine Module ===== ///
struct PWL_Engine
{
    SDL_GPUDevice* device;
};
bool PWL_SDLEvent(SDL_Event* event);

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

KHASH_MAP_INIT_STR(PWL_SceneMap_HasMap, PWL_Scene*)
struct PWL_Window
{
        char* name;
        SDL_Window* window;
        SDL_Renderer* render;
        khash_t(PWL_SceneMap_HasMap)* scene_map;
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
        kvec_t(char*) key_self_list;
};

SDL_Surface* PWL_GetSurfaceInSurfacePool(PWL_SurfacePool* pool, const char* name);
void PWL_SetSurfaceInSurfacePool(PWL_SurfacePool* pool, const char* name, SDL_Surface* surface);

/// ===== Scene Module ===== ///
struct PWL_Texture
{
        SDL_Texture* texture;
        bool is_visible;
        kvec_t(int) offset_list;
};

struct PWL_SceneLoopPackage
{
        PWL_Scene* scene;
};

struct PWL_Scene
{
        SDL_Renderer* render;
        void (*Loop)(char* next_scene, PWL_Scene* scene, float delta);
        kvec_t(PWL_Texture*) texture_layers;
        Uint16 loop_mode;
};

