#include "PWLibrary.h"
#include "_PWLibrary.h"

#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <stdlib.h>


PWLibrary_Surface* PWLibrary_SurfaceBuilder(void* SDL_surface)
{
        PWLibrary_Surface* nd_mbn_surface = malloc(sizeof(*nd_mbn_surface));
        if(nd_mbn_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: Can not create surface by malloc memory failed.");
        }

        SDL_Surface* surface = (SDL_Surface*) SDL_surface;
        if (surface == NULL)
        {
                PWLibrary_LogWarn("Warning: Can not create surface by SDL create surface failed.");
                free(nd_mbn_surface);
                return NULL;
        }
        nd_mbn_surface->surface = surface;

        return nd_mbn_surface;
}

PWLibrary_Surface* PWLibrary_CreateSurfaceByEmpty(int size_x, int size_y)
{
        SDL_Surface* surface = SDL_CreateSurface(size_x, size_y, SDL_PIXELFORMAT_RGBA8888);
        return PWLibrary_SurfaceBuilder(surface);
}
PWLibrary_Surface* PWLibrary_CreateSurfaceByImage(char* path)
{
        SDL_Surface* surface = IMG_Load(path);
        return PWLibrary_SurfaceBuilder(surface);
}
PWLibrary_Surface* PWLibrary_CreateSurfaceByText(PWLibrary_Font* font, int size, PWColor* color , char* text)
{
        SDL_Color sdl_color = {
                color->r, color->g, color->b, color->a
        };

        TTF_SetFontSize(font->font, size);
        SDL_Surface* surface = TTF_RenderText_Blended(font->font, text, strlen(text) , sdl_color);
        return PWLibrary_SurfaceBuilder(surface);
}
PWLibrary_Surface* PWLibrary_CloneSurface(PWLibrary_Surface* surface)
{
        if(surface->surface == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl surface(CloneSurface: surface) is null, operaction has been stopped.");
        }
        SDL_Surface* copy_surface = SDL_DuplicateSurface(surface->surface);
        return PWLibrary_SurfaceBuilder(copy_surface);
}

PWLibrary_Surface* PWLibrary_CloneSurfaceWithScale(PWLibrary_Surface* surface, float scale, PWLibrary_SurfaceScaleMode mode)
{
        SDL_Surface* get_surface = surface->surface;
        if(get_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl surface(CloneSurfaceWithScale: surface) is null, operaction has been stopped.");
        }

        SDL_Surface* result_surface = NULL;
        
        SDL_ScaleMode sdl_mode;
        switch (mode) 
        {
                case(PWLibrary_SURFACE_SCALE_LINEAR):
                        sdl_mode = SDL_SCALEMODE_LINEAR;
                        break;
                default:
                        sdl_mode = -1;
                        break;
        }

        SDL_Rect rect = {
                0, 0, 
                (int) (get_surface->w / scale), (int) (get_surface->h / scale)
        };
        
        SDL_BlitSurfaceScaled(surface->surface, NULL, result_surface, &rect, sdl_mode);
        if(result_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: use SDL_BlitSurfaceScaled create failed");
        }
        return PWLibrary_SurfaceBuilder(result_surface);

}

PWLibrary_Surface* PWLibrary_CloneSurfaceWithBlit(PWLibrary_Surface* surface, PWVec2i start, PWVec2i size)
{
        SDL_Surface* get_surface = surface->surface;
        if(get_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl surface(CloneSurfaceWithBlit: surface) is null, operaction has been stopped.");
        }

        SDL_Surface* result_surface = NULL;

        SDL_Rect rect = {
                start.x, start.y, size.x, size.y
        };

        SDL_Rect dst_rect = {
                0, 0, get_surface->w, get_surface->h
        };

        SDL_BlitSurface(get_surface, &rect, result_surface, &dst_rect);

        if(result_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: use SDL_BlitSurface create failed");
        }

        return PWLibrary_SurfaceBuilder(result_surface);
}

void PWLibrary_DrawRectInSurface(PWLibrary_Surface* surface, PWVec2i start, PWVec2i size, PWColor color)
{
        SDL_Surface* sdl_surface = surface->surface;
        if(sdl_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl surface(DrawRectInSurface) is null, operaction has been stopped.");
                return;
        }

        SDL_Rect rect = {
                start.x, start.y, size.x, size.y
        };

        Uint32 sdl_rgba_color = SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888), NULL, color.r, color.g, color.b, color.a);

        SDL_FillSurfaceRect(sdl_surface, &rect, sdl_rgba_color);
}

void PWLibrary_DrawSubsurfaceInSurface(PWLibrary_Surface* background_surface, PWLibrary_Surface* subsurface, PWVec2i start, PWVec2i size)
{
        SDL_Surface* sdl_bg_surface = background_surface->surface;
        if(sdl_bg_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl surface(DrawSubsurfaceInSurface: background_surface) is null, operaction has been stopped.");
                return;
        }

        SDL_Surface* sdl_sub_surface = subsurface->surface;
        if(sdl_bg_surface == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl surface(DrawSubsurfaceInSurface: subsurface) is null, operaction has been stopped.");
                return;
        }

        SDL_Rect rect = {
                start.x, start.y, size.x, size.y
        };

        SDL_BlitSurface(sdl_sub_surface, NULL, sdl_bg_surface, &rect);
}

void PWLibrary_DestroySurface(PWLibrary_Surface* surface)
{
        if(surface == NULL)
        {
                PWLibrary_LogWarn("Warning: surface is null! maybe it has been destroy.");
        }

        if (surface->surface == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl surface is null! maybe it has been destroy.");
        }
        else 
        {
                SDL_DestroySurface(surface->surface);
        }

        free(surface);   
}
