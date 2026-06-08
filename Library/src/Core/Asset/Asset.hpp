/*
 *
 *
 */
#pragma once


#include "Core/Core.hpp"
#include "Core/Foundation/PWEObject.hpp"
#include "SDL3/SDL_render.h"



// #include <SDL3_image/SDL_image.h>

#include <tuple>
#include <string>
#include "Core/Core.hpp"
#include "Core/Foundation/PWEVecMat.hpp"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

using namespace std;
// using namespace glm;
namespace PWEngine::Core
{
    class PWETexture : public PWERefCountPtr<SDL_Texture>
    {    
        public:
            
            PWETexture() = default;
            explicit PWETexture(SDL_Texture* tex) : PWERefCountPtr(tex, SDL_DestroyTexture){}

            tuple<int, int> getSize();
            static PWETexture createTextureByPath(SDL_Renderer* renderer, string path);
    };
    
}