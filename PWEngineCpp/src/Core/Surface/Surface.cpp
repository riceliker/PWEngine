#include "PWECore.hpp"

using namespace PWEngine::Type;
namespace PWEngine::Core
{
    PWESurface::PWESurface(PWESurface& surface)
    {
        setPtr(SDL_DuplicateSurface(surface.get()));
    }
    void PWESurface::drawRect(PWERectSize2 rect, PWEColor color)
    {
        SDL_Rect sdl_rect = SDL_Rect {
            .x = (int)rect.x, .y = (int)rect.y,
            .w = (int)rect.w, .h = (int)rect.h,
        };
        SDL_FillSurfaceRect(ptr.get(), &sdl_rect,SDL_MapSurfaceRGBA(ptr.get(), color.r, color.g, color.b, color.a));
    }
    void PWESurface::clearSurface(PWEColor color)
    {
        SDL_Rect sdl_rect = SDL_Rect {
            .x = 0, .y = 0,
            .w = (int)ptr.get()->w, .h = (int)ptr.get()->h,
        };
        SDL_FillSurfaceRect(ptr.get(), &sdl_rect,SDL_MapSurfaceRGBA(ptr.get(), color.r, color.g, color.b, color.a));
    }
    void PWESurface::drawSurface(PWESurface surface, PWEVec2T<uint> pos)
    {
        SDL_Rect dst_rect = { (int)pos.x, (int)pos.y, 0, 0 };
        SDL_BlitSurface(surface.get(), NULL, this->ptr.get(), &dst_rect);
    }
    void PWESurface::drawSurface(PWESurface surface, PWEVec2T<uint> pos, PWERectSize2 src_area)
    {
        SDL_Rect src_rect = { (int)src_area.x, (int)src_area.y, (int)src_area.w, (int)src_area.h };
        SDL_Rect dst_rect = { (int)pos.x, (int)pos.y, 0, 0 };
        SDL_BlitSurface(surface.get(), &src_rect, this->ptr.get(), &dst_rect);
    }
}