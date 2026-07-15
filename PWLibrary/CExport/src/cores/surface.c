#include "PWL.h"
#include "collections/klib/khash.h"
#include "collections/klib/kvec.h"
#include "core/core.h"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <_string.h>
#include <stdlib.h>

PWL_SurfacePool* PWL_CreateSurfacePool(void)
{
        PWL_SurfacePool* pool = (PWL_SurfacePool*)malloc(sizeof(PWL_SurfacePool));
        if (pool == NULL)
        {
                PWL_LogWarn("Warning: Can not malloc memory.");
                return NULL;
        }
        pool->surfaces_list = kh_init(PWL_SurfacePool_HasMap);
        kv_init(pool->key_self_list);
        if (pool->surfaces_list == NULL)
        {
                PWL_LogWarn("Warning: Can not init the hasmap.");
                return NULL;
        }
        return pool;
}

void PWL_DestroySurfacePool(PWL_SurfacePool* pool)
{
        if (pool == NULL)
        {
                PWL_LogWarn("Warning: surface pool is null! destroying was stopped.");
                return;
        }
        if (pool->surfaces_list == NULL)
        {
                PWL_LogWarn("Warning: hashmap in surface pool is null! destroying was stopped.");
                return;
        }
        int self_length = pool->key_self_list.n;
        for (int i = 0; i < self_length; ++i)
        {
                free(kv_A(pool->key_self_list, i));
        }
        kv_destroy(pool->key_self_list);
        khint_t k;
        for (k = kh_begin(pool->surfaces_list); k != kh_end(pool->surfaces_list); ++k)
        {
                if (kh_exist(pool->surfaces_list, k))
                {
                        SDL_DestroySurface(kh_val(pool->surfaces_list, k));
                } 
        }
        kh_destroy(PWL_SurfacePool_HasMap, pool->surfaces_list);
        free(pool);
}

SDL_Surface* PWL_GetSurfaceInSurfacePool(PWL_SurfacePool* pool, const char* name)
{
        if (pool == NULL)
        {
                PWL_LogWarn("Warning: The pool is NULL");
                return NULL;
        }
        if (pool->surfaces_list == NULL)
        {
                PWL_LogWarn("Warning: The hashmap is NULL");
                return NULL;
        }
        khint_t k = kh_get(PWL_SurfacePool_HasMap, pool->surfaces_list, name);
        SDL_Surface* surface = NULL;
        if (k != kh_end(pool->surfaces_list))
        {
                surface = kh_val(pool->surfaces_list, k);
        }
        return surface;
}

void PWL_SetSurfaceInSurfacePool(PWL_SurfacePool* pool, const char* name, SDL_Surface* surface)
{
        if (pool == NULL)
        {
                PWL_LogWarn("Warning: The pool is NULL");
                return;
        }
        if (pool->surfaces_list == NULL)
        {
                PWL_LogWarn("Warning: The hashmap is NULL");
                return;
        }
        int absent;
        char* name_h = strdup(name);
        khint_t k = kh_put(PWL_SurfacePool_HasMap, pool->surfaces_list, name_h, &absent);
        if (absent)
        {
                kh_key(pool->surfaces_list, k) = name_h;
                kh_val(pool->surfaces_list, k) = surface;
                kv_push(char*, pool->key_self_list, name_h);
        }
        else
        {
                SDL_Surface* old_surface = kh_val(pool->surfaces_list, k);
                SDL_DestroySurface(old_surface);
                kh_val(pool->surfaces_list, k) = surface;
                free(name_h);
        }
}

void PWL_AddSurfaceByEmptyInPool(PWL_SurfacePool* pool, char* name, PW_vmt_Vec2i size)
{
        SDL_Surface* surface = SDL_CreateSurface(size.x, size.y, SDL_PIXELFORMAT_RGBA8888);
        if (surface == NULL)
        {
                PWL_LogWarn("Warning: Create empty surface failed.");
                return;
        }
        PWL_SetSurfaceInSurfacePool(pool, name, surface);
}

void PWL_AddSurfaceByIamgeInPool(PWL_SurfacePool* pool, char* name, char* path)
{
        SDL_Surface* surface = IMG_Load(path);
        if (surface == NULL)
        {
                PWL_LogWarn("Warning: Create empty surface failed.");
                return;
        }
        PWL_SetSurfaceInSurfacePool(pool, name, surface);
}

void PWL_AddSurfaceByTextInPool(PWL_SurfacePool* pool, char* name, PWL_Font* font, int size, PW_vmt_Color* color , char* text)
{
        SDL_Color sdl_color = {color->r, color->g, color->b, color->a};
        if (font->font == NULL)
        {
                PWL_LogWarn("Warning: Font is NULL.");
                return;
        }
        TTF_SetFontSize(font->font, size);
        SDL_Surface* surface = TTF_RenderText_Blended(font->font, text, strlen(text) , sdl_color);
        if (surface == NULL)
        {
                PWL_LogWarn("Warning: Can not create surface by text.");
                return;
        }
        PWL_SetSurfaceInSurfacePool(pool, name, surface);
}

void PWL_AddSurfaceByCopySurfaceInPool(PWL_SurfacePool* pool, char* name, PWL_Surface surface)
{
        if (surface.surface == NULL)
        {
                PWL_LogWarn("Warning: Can not create surface by text.");
                return;
        }
        PWL_SetSurfaceInSurfacePool(pool, name, surface.surface);
}

PWL_Surface PWL_ExportSurfaceByCopySurfaceInPool(PWL_SurfacePool* pool, char* name)
{
        PWL_Surface surface;
        surface.surface = PWL_GetSurfaceInSurfacePool(pool, name);
        return surface;
}

void PWL_CloneSurfaceInPool(PWL_SurfacePool* pool, char* from_name, char* new_name)
{
        SDL_Surface* from_surface = PWL_GetSurfaceInSurfacePool(pool, from_name);
        SDL_Surface* copy_surface = SDL_DuplicateSurface(from_surface);
        PWL_SetSurfaceInSurfacePool(pool, new_name, copy_surface);
}

void PWL_CloneSurfaceWithScaleInPool(PWL_SurfacePool* pool, char* from_name, char* new_name, float scale, PWL_SurfaceScaleMode mode)
{
        SDL_Surface* from_surface = PWL_GetSurfaceInSurfacePool(pool, from_name);
        if (from_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(CloneSurfaceWithScale: surface) is null, operaction has been stopped.");
        }
        SDL_Surface* copy_surface = NULL;
        SDL_ScaleMode sdl_mode;
        switch (mode) 
        {
                case(PWL_SURFACE_SCALE_LINEAR):
                        sdl_mode = SDL_SCALEMODE_LINEAR;
                        break;
                default:
                        sdl_mode = -1;
                        break;
        }
        SDL_Rect rect = {
                0, 0, 
                (int) (from_surface->w / scale), (int) (from_surface->h / scale)
        };
        SDL_BlitSurfaceScaled(from_surface, NULL, copy_surface, &rect, sdl_mode);
        if(copy_surface == NULL)
        {
                PWL_LogWarn("Warning: use SDL_BlitSurfaceScaled create failed");
        }
        PWL_SetSurfaceInSurfacePool(pool, new_name, copy_surface);
}

void PWL_CloneSurfaceWithBlitInPool(PWL_SurfacePool* pool, char* from_name, char* new_name, PW_vmt_Vec2i start, PW_vmt_Vec2i size)
{
        SDL_Surface* from_surface = PWL_GetSurfaceInSurfacePool(pool, from_name);
        if(from_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(CloneSurfaceWithBlit: surface) is null, operaction has been stopped.");
        }
        SDL_Surface* copy_surface = NULL;
        SDL_Rect rect = {
                start.x, start.y, size.x, size.y
        };
        SDL_Rect dst_rect = {
                0, 0, from_surface->w, from_surface->h
        };
        SDL_BlitSurface(from_surface, &rect, copy_surface, &dst_rect);
        if(copy_surface == NULL)
        {
                PWL_LogWarn("Warning: use SDL_BlitSurface create failed");
        }
        PWL_SetSurfaceInSurfacePool(pool, new_name, copy_surface);
}

void PWL_DrawRectToSurfaceInPool(PWL_SurfacePool* pool, char* name, PW_vmt_Vec2i start, PW_vmt_Vec2i size, PW_vmt_Color color)
{
        SDL_Surface* sdl_surface = PWL_GetSurfaceInSurfacePool(pool, name);
        if(sdl_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(DrawRectInSurface) is null, operaction has been stopped.");
                return;
        }
        SDL_Rect rect = {
                start.x, start.y, size.x, size.y
        };
        Uint32 sdl_rgba_color = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888), NULL, color.r, color.g, color.b, color.a);
        SDL_FillSurfaceRect(sdl_surface, &rect, sdl_rgba_color);
}

void PWL_DrawSubsurfaceToSurfaceInPool(PWL_SurfacePool* pool, char* background_surface_name, char* subsurface_name, PW_vmt_Vec2i start, PW_vmt_Vec2i cut_size)
{
        SDL_Surface* sdl_bg_surface = PWL_GetSurfaceInSurfacePool(pool, background_surface_name);
        if(sdl_bg_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(DrawSubsurfaceInSurface: background_surface) is null, operaction has been stopped.");
                return;
        }
        SDL_Surface* sdl_sub_surface = PWL_GetSurfaceInSurfacePool(pool, subsurface_name);
        if(sdl_bg_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(DrawSubsurfaceInSurface: subsurface) is null, operaction has been stopped.");
                return;
        }
        SDL_Rect rect = {
                start.x, start.y, cut_size.x, cut_size.y
        };
        SDL_BlitSurface(sdl_sub_surface, NULL, sdl_bg_surface, &rect);
}