#include "PWL.h"
#include "core/core.h"


#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"

#include "SDL3/SDL_platform.h"

#include "SDL3/SDL_render.h"

#include "SDL3/SDL_version.h"
#include "SDL3/SDL_video.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <stdlib.h>
#include <string.h>

// log module

void PWL_LogInfo(const char *fmt)
{
        SDL_LogInfo(0, "%s", fmt);
        printf(PWE_CONSOLE_RESET "%s" PWE_CONSOLE_RESET, fmt);
}

void PWL_LogWarn(const char *fmt)
{
        SDL_LogWarn(0, "%s", fmt);
        SDL_LogWarn(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_YELLOW "%s" PWE_CONSOLE_RESET, fmt);      
}

void PWL_LogError(const char *fmt)
{
        SDL_LogError(0, "%s", fmt);
        SDL_LogError(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_RED "%s" PWE_CONSOLE_RESET, fmt);
}

void PWL_PreloadInfo(void* device)
{
        printf("PWEngine v0.0.1 beta. Copyright by riceliker and constructors. Open source license: LGPL2.0\n");
        printf("SDL Version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
        printf("Runing platform: %s\n", SDL_GetPlatform());
        printf("Graphic API: %s\n", SDL_GetGPUDeviceDriver(device));
}

// engine module

PWL_Engine* PWL_CreateEngine(void)
{
        PWL_Engine *engine = (PWL_Engine*)malloc(sizeof(*engine));
        if (!TTF_Init())
        {
                PWL_LogError("Error: Init TTF failed.");
        }
         
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
                PWL_LogWarn("Error: Init SDL failed");
        } 

        SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
        engine->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

        if (engine->device == NULL)
        {
                PWL_LogError("Error: can not create device.");
        }

        PWL_PreloadInfo(engine->device);
                
        return engine;
}

void PWL_DestroyEngine(PWL_Engine* engine)
{
        if(engine == NULL)
        {
                PWL_LogWarn("Warning: engine is null! destroying was stopped.");
        }
        free(engine);
}

PWL_Window* PWL_CreateWindow(PWL_Engine *engine, PWL_WindowInfo* info)
{
        PWL_Window *window = (PWL_Window*) malloc(sizeof(*window));
        if (window == NULL)
        {
                PWL_LogWarn("Warning: can not malloc window.");
                free(window);
                return NULL;
        }

        size_t len = strlen(info->name);
        window->name = (char*)malloc(len + 1);
        if (window->name == NULL) 
        {
                free(window);
                return NULL;
        }
        strcpy(window->name, info->name);
                
        window->window = SDL_CreateWindow(info->name, info->physic_w, info->physic_h, info->mode);
        if (window->window == NULL)
        {
                PWL_LogWarn("Warning: can not create window.");
                free(window);
                return NULL;
        }

        SDL_ReleaseWindowFromGPUDevice(engine->device, window->window);

        if (SDL_ClaimWindowForGPUDevice(engine->device, window->window) == false)
        {
                PWL_LogWarn("Warning: can not bind window and device.");
                free(window);
                return NULL;
        }
        
        window->render = SDL_CreateGPURenderer(engine->device, window->window);
        if (window->render == NULL)
        {
                PWL_LogWarn("Warning: can not create render.");
                free(window);
                return NULL;
        }
        if (SDL_SetRenderLogicalPresentation(window->render, info->logical_h, info->logical_w, SDL_LOGICAL_PRESENTATION_LETTERBOX) == false)
        {
                
        }
        return window;
}

void PWL_DestroyWindow(PWL_Window *window)
{
        if (window == NULL)
        {
                PWL_LogWarn("Warning: window is null! maybe it has been destroy.");
                return;
        }

        if(window->name == NULL)
        {
                PWL_LogWarn("Warning: name is null! maybe it has been destroy.");
        }
        else 
        {
                free(window->name);
        }
        
        if(window->window == NULL)
        {
                PWL_LogWarn("Warning: sdl window is null! maybe it has been destroy.");
        }
        else 
        {
                SDL_DestroyWindow(window->window);
        }
        
        if(window->render == NULL)
        {
               PWL_LogWarn("Warning: sdl render is null! maybe it has been destroy."); 
        }
        else 
        {
                SDL_DestroyRenderer(window->render);
        }
        
        free(window);
}

void PWL_EngineRun(PWL_Engine* engine)
{
        SDL_Event event;
        bool is_engine_running = true;
        while (is_engine_running) 
        {
                while(SDL_PollEvent(&event)) 
                {
                        switch (event.type) 
                        {
                                case SDL_EVENT_QUIT:
                                        is_engine_running = false;
                                        break;
                                default:
                                        break;
                        }
                }
        }
}