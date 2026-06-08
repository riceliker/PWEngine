#include "Core/Scene/Scene.hpp"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

namespace PWEngine::Core
{
    void PWEUIScene::staticDrawPoint(PWEStaticPoint point)
    {
        SDL_SetRenderDrawColor(renderer, point.color.r, point.color.g, point.color.b, point.color.a);
        SDL_RenderPoint(this->renderer, point.pos.x, point.pos.y);
    }
    void PWEUIScene::staticDrawLine(PWEStaticLine line)
    {
        SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
        SDL_RenderLine(this->renderer, line.pos.start().x, line.pos.start().y, line.pos.end().x, line.pos.end().y);
    }
    void PWEUIScene::staticDrawRect(PWEStaticRect rect)
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
    void PWEUIScene::staticDrawTexture(PWEStaticTexture texture)
    {
        SDL_FRect dstRect = {
            .x = texture.pos.start().x, .y = texture.pos.start().y,
            .w = texture.pos.size().x, .h = texture.pos.size().y
        };

        SDL_SetTextureScaleMode(texture.texture.get(), texture.scale_mode);
        SDL_RenderTexture(this->renderer, texture.texture.get(), nullptr, &dstRect);
    }
}