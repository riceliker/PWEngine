#include "PWECore.hpp"

namespace PWEngine::Core
{
    PWECanvasScene::PWECanvasScene(PWEWindowInfo info)
    {
        this->renderer = info.renderer;
    }
    void PWECanvasScene::clearCanvas(PWEColor color)
    {
        SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(this->renderer);
    }
    void PWECanvasScene::addSurface(PWESurface surface)
    {
        PWESurface a = surface;
        // surfaces.push_back(surface);
    }
    void PWECanvasScene::submitCanvas()
    {
        SDL_RenderPresent(this->renderer);   
    }
}