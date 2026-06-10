/*
 * PWEAsset is a class with reference count, store the basic Asset in engine.
 * Because of extending the PWERefCountObject, you will never to delete Asset by yourself.
 * Notice: DO NOT DELETE PTR, it must be deleted by itself.
 */
#pragma once


#include "Core/Foundation/PWEColor.hpp"
#include "Core/Foundation/PWEObject.hpp"
#include "Core/Foundation/PWEVecMat.hpp"
#include "Core/Foundation/PWERect.hpp"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"


#include <cstddef>
#include <tuple>
#include <string>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

using namespace std;
namespace PWEngine::Core
{
    typedef struct
    {
        PWEVec2T<int> size;
        SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888;
    } PWESurfaceInfo;
    [[deprecated("use PWESurfaceFactory")]];
    class PWEFont : public PWERefCountPtr<TTF_Font>
    {
        public:
            PWEFont(PWEFont&& other) noexcept : PWERefCountPtr<TTF_Font>(std::move(other))
            {
                if (other.get()) *this = other;
            }

            PWEFont& operator=(PWEFont&& other) noexcept
            {
                if (this != &other) PWERefCountPtr<TTF_Font>::operator=(other);
                return *this;
            }

            PWEFont(const PWEFont&) = default;
            PWEFont& operator=(const PWEFont&) = default;
            static void Destroy(TTF_Font* font)
            {
                if(font) TTF_CloseFont(font);
            }
            explicit PWEFont(TTF_Font* font) : PWERefCountPtr(font, Destroy){}
            PWEFont() : PWERefCountPtr(nullptr, Destroy) {}
            PWEFont(const string& path, int size) : PWEFont(nullptr)
            {
                TTF_Font* t_font = TTF_OpenFont(path.c_str(), size);
                if(t_font == nullptr) SDL_LogWarn(-1, "Asset Font not found path(%s)", path.c_str());
                else *this = PWEFont(t_font);
            }
            
    };
    
    class PWESurface : public PWERefCountPtr<SDL_Surface>
    {
        private:
        public:
            PWESurface() = default;
            explicit PWESurface(SDL_Surface* suf) : PWERefCountPtr(suf, SDL_DestroySurface){}
            
            PWESurface(PWESurface &surface);

            void clearSurface(PWEColor color);
            void drawRect(PWERectSize2 rect, PWEColor color);
            void drawSurface(PWESurface surface, PWEVec2T<uint> pos);
            void drawSurface(PWESurface surface, PWEVec2T<uint> pos, PWERectSize2 src_area);
            
    };
    [[deprecated("use PWESurfaceFactory")]];
    class PWETexture : public PWERefCountPtr<SDL_Texture>
    {    
        public:
            PWETexture() = default;
            explicit PWETexture(SDL_Texture* tex) : PWERefCountPtr(tex, SDL_DestroyTexture){}

            tuple<int, int> getSize();  
            [[deprecated]]
            PWETexture(SDL_Renderer* renderer, string path);
            [[deprecated("Use render surface")]]
            PWETexture(SDL_Renderer* renderer, PWESurface surface);   
    };

    class PWESurfaceFactory
    {
        public:
            static PWESurface createSurfaceByImage(string path);
            static PWESurface createSurfaceByText(PWEFont font, PWEColor color, string text);
            static PWESurface createEmptySurface(PWEVec2T<uint> size, PWEColor color);
            static PWESurface copySurface(PWESurface surface);
    };
    
    
}