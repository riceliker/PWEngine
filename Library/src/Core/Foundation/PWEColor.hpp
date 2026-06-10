#pragma once

#include "SDL3/SDL_pixels.h"
#include "Core/Foundation/Type.hpp"
namespace PWEngine::Core 
{  
    struct PWEColor
    {
        public:
            uchar r; uchar g; uchar b; uchar a;
            SDL_Color forSDL()
            {
                SDL_Color color{
                    .r = (Uint8)r, .g = (Uint8)g, .b = (Uint8)b, .a = (Uint8)a
                };
                return color;
            }
            Uint32 forSDL32()
            {
                return SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888), NULL, r, g, b, a);
            }

    };
}