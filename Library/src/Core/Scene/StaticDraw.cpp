#include "Core/Foundation/PWEVecMat.hpp"
#include "Core/Scene/Scene.hpp"
#include "Core/Surface/Surface.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

namespace PWEngine::Core
{
    void PWECanvasScene::staticDrawPoint(PWEStaticPoint point)
    {
        SDL_SetRenderDrawColor(renderer, point.color.r, point.color.g, point.color.b, point.color.a);
        SDL_RenderPoint(this->renderer, point.pos.x, point.pos.y);
    }
    void PWECanvasScene::staticDrawLine(PWEStaticLine line)
    {
        SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
        SDL_RenderLine(this->renderer, line.pos.start().x, line.pos.start().y, line.pos.end().x, line.pos.end().y);
    }
    void PWECanvasScene::staticDrawRect(PWEStaticRect rect)
    {
        SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
        SDL_FRect SDL_rect = {
            .x=rect.rect.start().x, .y=rect.rect.end().y,
            .w=rect.rect.size().x, .h=rect.rect.size().y
        };
        if(rect.is_full)
            SDL_RenderFillRect(this->renderer, &SDL_rect);
        else
            SDL_RenderRect(this->renderer, &SDL_rect);
    }
    void PWECanvasScene::draw(PWESurface surface, PWEVec2T<uint> pos)
    {
        SDL_Texture* texture =  SDL_CreateTextureFromSurface(this->renderer, surface.get());
        
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