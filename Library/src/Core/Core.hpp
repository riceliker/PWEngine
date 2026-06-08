#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>

namespace PWEngine::Core
{
    typedef enum
    {
        Vulkan, Metal, Direct3D, OpenGL
    } PWEngineGraphicAPI;
    typedef struct
    {
        SDL_GPUDevice* device;
        SDL_Window* window;
        SDL_Renderer* renderer;
    } PWEWindowInfo;
    
}