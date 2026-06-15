#include "PWECore.hpp"

namespace PWEngine::Core
{
    void PWECanvasScene::draw(PWESurface surface, PWEVec2T<uint> pos)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface.get());
        
        SDL_FRect dstRect = {
            .x = (float)pos.x, .y = (float)pos.y,
            .w = (float)surface->w, .h = (float)surface->h
        };

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
        SDL_RenderTexture(this->renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }
    void PWECanvasScene::draw(PWESurface surface, PWEVec2T<uint> pos, PWEVec2T<uint> size)
    {
        SDL_Texture* texture =  SDL_CreateTextureFromSurface(this->renderer, surface.get());
        
        SDL_FRect rect = {
            .x = (float)pos.x, .y = (float)pos.y,
            .w = (float)size.x, .h = (float)size.y
        };

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
        SDL_RenderTexture(this->renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }
}