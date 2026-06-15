#include "PWECore.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"


namespace PWEngine::Core
{
    tuple<int, int> PWETexture::getSize()
    {
        float w = -1;
        float h = -1;
        if (this->ptr) SDL_GetTextureSize(this->ptr.get(), &w, &h);
        return {(int)w, (int)h};
    }
    PWETexture::PWETexture(SDL_Renderer* renderer, string path)
    {
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if(!texture)
            SDL_LogWarn(-1, "Texture can not be render From->(%s), SDL->(%s)", path.c_str(), SDL_GetError());
        setPtr(texture);
    }
    PWETexture::PWETexture(SDL_Renderer* renderer, PWESurface surface)
    {
       setPtr(SDL_CreateTextureFromSurface(renderer, surface.get()));
    }
}