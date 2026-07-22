#pragma once

#include "SDL3_ttf/SDL_ttf.h"

namespace PWL::Core
{
    class Font
    {
    public:
        TTF_Font* ptr;
        Font(TTF_Font* ptr)
        {
            this->ptr = ptr;
        }
        void Destroy()
        {
            TTF_CloseFont(this->ptr);
        }
    };
}