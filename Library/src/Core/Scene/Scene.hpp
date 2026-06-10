/*
 *
 *
 */

#pragma once
#include "Core/Core.hpp"
#include "Core/Surface/Surface.hpp"

#include "Core/Foundation/PWERect.hpp"
#include "Core/Foundation/PWEColor.hpp"
#include "Core/Foundation/PWEVecMat.hpp"
#include "Core/Foundation/Type.hpp"
#include "SDL3/SDL_render.h"

using namespace std;
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
    class IPWEScene
    {
        public:
            virtual string loop() = 0;
    };
    class PWECanvasScene : public IPWEScene
    {
        protected:
            SDL_Renderer* renderer;
        public:
            PWECanvasScene(PWEWindowInfo info)
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
            [[deprecated("Use static draw to draw so many object is not suggested, use surface first.")]]
            void staticDrawPoint(PWEStaticPoint point);
            [[deprecated("Use static draw to draw so many object is not suggested, use surface first.")]]
            void staticDrawLine(PWEStaticLine line);
            [[deprecated("Use static draw to draw so many object is not suggested, use surface first.")]]
            void staticDrawRect(PWEStaticRect rect);


            void draw(PWESurface surface, PWEVec2T<uint> pos);
            void draw(PWESurface surface, PWEVec2T<uint> pos, PWEVec2T<uint> size);

    };
    
}