#include "Core/Surface/Surface.hpp"
#include "Core/Foundation/PWEColor.hpp"

namespace PWEngine::Core 
{
    PWESurface PWESurfaceFactory::createSurfaceByImage(string path)
    {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if(surface) SDL_LogWarn(-1, "Texture not found path(%s)", path.c_str());
        return PWESurface(surface);
    }
    PWESurface PWESurfaceFactory::createSurfaceByText(PWEFont font, PWEColor color, string text)
    {
        SDL_Surface* surface = TTF_RenderText_Blended(font.get(), text.c_str(), text.length(), color.forSDL());
        if(surface) SDL_LogWarn(-1, "The font can not be used.");
        return PWESurface(surface);
        
    }
    PWESurface PWESurfaceFactory::createEmptySurface(PWEVec2T<uint> size, PWEColor color)
    {
        SDL_Surface* surface = SDL_CreateSurface(size.x, size.y, SDL_PIXELFORMAT_RGBA8888);
        if(surface) SDL_LogWarn(-1, "create surface error.");
        SDL_FillSurfaceRect(surface, nullptr, color.forSDL32());
        return PWESurface(surface);
    }
    PWESurface PWESurfaceFactory::copySurface(PWESurface surface)
    {
        return PWESurface(SDL_DuplicateSurface(surface.get()));
    }
}