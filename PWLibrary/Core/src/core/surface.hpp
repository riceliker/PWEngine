#pragma once

#include "core/log.hpp"
#include "core/collections.hpp"
#include "core/font.hpp"

#include "SDL3/SDL_surface.h"
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
namespace PWL::Core
{
    class Surface
    {
    private:
        SDL_Surface* ptr;
        void _DrawPixel(uint32_t x, uint32_t y, uint32_t color)
        {
            if (this->GetIsCreate())
            {
                SDL_Surface* surface = this->ptr;
                if (surface == NULL || x >= surface->w || y >= surface->h)
                    return;
                uint8_t* pixel = (uint8_t*)surface->pixels + y * surface->pitch + x;
                *(uint8_t*)pixel = color;
            }
            return;
        }
        void _DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
        {
            if (this->GetIsCreate())
            {
                SDL_Surface* surface = this->ptr;
                int dx = abs(x1 - x0);
                int dy = abs(y1 - y0);
                int sx = x0 < x1 ? 1 : -1;
                int sy = y0 < y1 ? 1 : -1;
                int err = dx - dy;

                while (true)
                {
                    this->_DrawPixel(x0, x0, color);
                    if (x0 == x1 && y0 == y1) break;
                    int e2 = err * 2;
                    if (e2 > -dy) { err -= dy; x0 += sx; }
                    if (e2 < dx) { err += dx; y0 += sy; }
                }
            }
        }
        void _DrawRect(Vec2<uint32_t> start, Vec2<uint32_t> size, uint32_t color)
        {
            if (this->GetIsCreate())
            {
                SDL_Surface* surface = this->ptr;
                SDL_Rect rect = {
                    (int)start.x, (int)start.y, (int)size.x, (int)size.y
                };

                SDL_FillSurfaceRect(surface, &rect, color);
            }
        }
        void _DrawCircle(Vec2<uint32_t> pos, uint32_t r, uint32_t color)
        {
            if (this->GetIsCreate())
            {
                SDL_Surface* surface = this->ptr;
                int cx = pos.x;
                int cy = pos.y;
                
                vector<int> row;

                int x = 0;
                int y = r;
                int d = 1 - r;

                while (x <= y)
                {
                    row[y] = x;
                        
                    row[x] = y;


                    x++;
                    if (d < 0)
                    {
                        d += 2 * x - 1;
                    }
                    else
                    {
                        y--;
                        d += 2 * (x - y) - 1;
                    }
                }

                int w0 = row[0];
                this->_DrawLine(cx - w0, cy, cx + w0, cy, color);

                for (int dy = 1; dy <= r; ++dy)
                {
                    int w = row[dy];
                    this->_DrawLine(cx - w, cy + dy, cx + w, cy + dy, color);
                    this->_DrawLine(cx - w, cy - dy, cx + w, cy - dy, color);
                }
            }
        }
    public:
        Surface(SDL_Surface* move_surface)
        {
            if (move_surface != nullptr)
            {
                this->ptr = move_surface;
            }
        }
        Surface(Surface* surface)
        {
            this->ptr = SDL_DuplicateSurface(surface->ptr);
        }
        Surface()
        {
            this->ptr = nullptr;
        }
        ~Surface()
        {
            this->DestroyInside();
        }
        bool GetIsCreate()
        {
            return this->ptr == nullptr;
        }
        void DestroyInside()
        {
            if (this->GetIsCreate())
            {
                SDL_DestroySurface(this->ptr);
            }
        }
        SDL_Surface* const GetPtr()
        {
            return this->ptr;
        }
        Vec2<uint32_t> GetSize()
        {
            uint32_t w = this->ptr->w;
            uint32_t h = this->ptr->h;
            Vec2<uint32_t> size = Vec2<uint32_t>(w, h); 
            return size;
        }
        void DrawLine(Vec2<uint32_t> position, Vec2<uint32_t> size, PWL_Color color)
        {
            if (this->GetIsCreate())
            {
                uint32_t _color = SDL_MapRGBA(SDL_GetPixelFormatDetails(
                SDL_PIXELFORMAT_RGBA8888), NULL,
                color.r, color.g, color.b, color.a
                );
                SDL_Surface* surface = this->ptr;
                this->_DrawLine(position.x, position.y, size.x, size.y, _color);
            }
        }
        void PWL_DrawBoxInSurfaces(Vec2<uint32_t> position, Vec2<uint32_t> size, uint32_t radius, PWL_Color color)
        {
            if (this->GetIsCreate())
            {
                uint32_t _color = SDL_MapRGBA(SDL_GetPixelFormatDetails(
                    SDL_PIXELFORMAT_RGBA8888), NULL,
                    color.r, color.g, color.b, color.a
                );

                /* Standard Circle*/
                if (size.x == 0 && size.y == 0)
                {
                    this->_DrawCircle(position, radius, _color);
                    return;
                }

                /* Standard Rect */
                if (radius == 0)
                {
                        this->_DrawRect(position, size, _color);
                        return;
                }
                /* Round Rect */
                radius = min(radius , min(size.x/2, size.y/2));;
                Vec2<uint32_t> c0 =  {position.x + radius,position.y + radius};
                this->_DrawCircle(c0, radius, _color);
                Vec2<uint32_t> c1 =  {position.x + size.x - radius,position.y + radius};
                this->_DrawCircle(c1, radius, _color);
                Vec2<uint32_t> c2 =  {position.x + radius,position.y + size.y - radius};
                this->_DrawCircle(c2, radius, _color);
                Vec2<uint32_t> c3 =  {position.x + size.x - radius,position.y + size.y - radius};
                this->_DrawCircle(c3, radius, _color);

                Vec2<uint32_t> rp0 = {position.x + radius, position.y};
                Vec2<uint32_t> rs0 = {size.x - 2 * radius, size.y};
                this->_DrawRect(rp0, rs0, _color);

                Vec2<uint32_t> rp1 = {position.x, position.y + radius};
                Vec2<uint32_t> rs1 = {size.x, size.y  - 2 * radius};
                this->_DrawRect(rp0, rs0, _color);
            }
        }
    };

    class SurfacePool
    {
    private:
        unordered_map<string, Surface*> dict;
    public:
        void Destroy()
        {
            for(const auto& object: this->dict)
            {
                auto surface = object.second;
                if (surface != nullptr)
                {
                    delete surface;
                }
            }
        }

        const optional<const Surface* const> Get(string name)
        {
            if(this->dict.find(name) == this->dict.end())
            {
                return nullopt;
            }
            return this->dict[name];
        }

        const optional<const Surface* const> AddSurfaceByEmpty(string name, Vec2<uint32_t> size)
        {
            Surface* surface = new Surface();
            this->dict.insert({name, surface});
            return surface;
        }

        const optional<const Surface* const> PWL_AddSurfaceByTextInPool(string name, optional<Font*> font, uint32_t size, PWL_Color* color , string text)
        {
            if (font.has_value())
            {
                TTF_Font* _font = font.value()->ptr;
                SDL_Color sdl_color = {color->r, color->g, color->b, color->a};
                TTF_SetFontSize(_font, size);
                SDL_Surface* new_surface = TTF_RenderText_Blended(_font, text.c_str(), strlen(text.c_str()) , sdl_color);
                if (new_surface == NULL)
                {
                    Log::LogWarn("The file not found");
                }
                Surface* surface = new Surface(new_surface);
                this->dict.insert({name, surface});
                return surface;
            }
            return nullopt;
        }

        const optional<const Surface* const> AddSurfaceByPNG(string name, string path)
        {
            SDL_Surface* new_surface = SDL_LoadPNG(path.c_str());
            if (new_surface == NULL)
            {
                Log::LogWarn("The file not found");
            }
            Surface* surface = new Surface(new_surface);
            this->dict.insert({name, surface});
            return surface;
        }

        const optional<const Surface* const> AddSurfaceByCopy(string name, optional<Surface*> surface)
        {
            if (surface.has_value())
            {
                Surface* new_surface = new Surface(surface.value());
                this->dict.insert({name, new_surface});
                return surface;
            }
            return nullopt;
        }

        const optional<const Surface* const> AddSurfaceByScale(string name, optional<Surface*> surface, float scale)
        {
            if (surface.has_value())
            {
                SDL_Surface* from_surface = surface.value()->GetPtr();
                SDL_Surface* copy_surface = nullptr;
                SDL_ScaleMode sdl_mode = SDL_SCALEMODE_LINEAR;
                SDL_Rect rect = {
                        0, 0, 
                        (int) (from_surface->w / scale), (int) (from_surface->h / scale)
                };
                SDL_BlitSurfaceScaled(from_surface, NULL, copy_surface, &rect, sdl_mode);
                Surface* new_surface = new Surface(copy_surface);
                this->dict.insert({name, new_surface});
                return surface;
            }
            return nullopt;
        }

        const optional<const Surface* const> AddSurfaceByBlit(string name, optional<Surface*> surface, Vec2<uint32_t> start, Vec2<uint32_t> size)
        {
            if (surface.has_value())
            {
                SDL_Surface* from_surface = surface.value()->GetPtr();
                SDL_Surface* copy_surface = nullptr;
                SDL_Rect rect = {
                        (int)start.x, (int)start.y, (int)size.x, (int)size.y
                };
                SDL_Rect dst_rect = {
                        0, 0, from_surface->w, from_surface->h
                };
                SDL_BlitSurface(from_surface, &rect, copy_surface, &dst_rect);
                Surface* new_surface = new Surface(copy_surface);
                this->dict.insert({name, new_surface});
                return surface;
            }
            return nullopt;
        }
    };
}