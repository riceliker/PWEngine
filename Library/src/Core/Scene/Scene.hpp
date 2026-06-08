/*
 *
 *
 */

#pragma once
#include "Core/Core.hpp"
#include "Core/Asset/Asset.hpp"

#include "Core/Foundation/PWEAutoRect.hpp"
#include "Core/Foundation/PWEColor.hpp"
#include "Core/Foundation/PWEVecMat.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "glm/ext/vector_float4.hpp"
#include "glm/ext/vector_float2.hpp"
#include <span>

using namespace std;
using namespace glm;
using namespace PWEngine::Core;
namespace PWEngine::Core
{
    
    typedef struct
    {
        PWEVec2 pos;
        PWEColor color;
    } PWEStaticPoint;
    typedef struct
    {
        PWEAutoRect pos;
        PWEColor color;
    } PWEStaticLine;
    typedef struct
    {
        PWEAutoRect rect;
        PWEColor color;
        bool is_full;
    } PWEStaticRect;
    typedef struct
    {
        PWETexture texture;
        PWEAutoRect pos;
        SDL_ScaleMode scale_mode = SDL_SCALEMODE_LINEAR;
    } PWEStaticTexture;
    class IPWEScene
    {
        public:
            virtual void init() = 0;
            virtual string loop() = 0;
    };
    class PWEUIScene : public IPWEScene
    {
        protected:
            SDL_Renderer* renderer;
        public:
            PWEUIScene(PWEWindowInfo info)
            {
                this->renderer = info.renderer;

                int w, h;
                SDL_GetWindowSizeInPixels(info.window, &w, &h);
            }
            void clearCanvas(PWEColor color)
            {
                SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
                SDL_RenderClear(this->renderer);
            }
            void submitCanvas()
            {
                SDL_RenderPresent(this->renderer);   
            }

            void staticDrawPoint(PWEStaticPoint point);
            void staticDrawLine(PWEStaticLine line);
            void staticDrawRect(PWEStaticRect rect);
            void staticDrawTexture(PWEStaticTexture texture);


    };
    
}