#include "PWLibrary.h"
#include "_PWLibrary.h"


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

void PWLibrary_LogInfo(const char *fmt)
{
        SDL_LogInfo(0, "%s", fmt);
        printf(PWE_CONSOLE_RESET "%s" PWE_CONSOLE_RESET, fmt);
}

void PWLibrary_LogWarn(const char *fmt)
{
        SDL_LogWarn(0, "%s", fmt);
        SDL_LogWarn(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_YELLOW "%s" PWE_CONSOLE_RESET, fmt);      
}

void PWLibrary_LogError(const char *fmt)
{
        SDL_LogError(0, "%s", fmt);
        SDL_LogError(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_RED "%s" PWE_CONSOLE_RESET, fmt);
}

void PWLibrary_PreloadInfo(void* device)
{
        printf("PWEngine v0.0.1 beta. Copyright by riceliker and constructors. Open source license: LGPL2.0\n");
        printf("SDL Version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
        printf("Runing platform: %s\n", SDL_GetPlatform());
        printf("Graphic API: %s\n", SDL_GetGPUDeviceDriver(device));
}

// engine module

PWLibrary_Engine* PWLibrary_CreateEngine(void)
{
        PWLibrary_Engine *engine = (PWLibrary_Engine*)malloc(sizeof(*engine));
        if (!TTF_Init())
        {
                PWLibrary_LogError("Error: Init TTF failed.");
        }
         
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
                PWLibrary_LogWarn("Error: Init SDL failed");
        } 

        SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
        engine->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

        if (engine->device == NULL)
        {
                PWLibrary_LogError("Error: can not create device.");
        }

        PWLibrary_PreloadInfo(engine->device);
                
        return engine;
}

void PWLibrary_DestroyEngine(PWLibrary_Engine* engine)
{
        if(engine == NULL)
        {
                PWLibrary_LogWarn("Warning: engine is null! destroying was stopped.");
        }
        free(engine);
}

PWLibrary_Window* PWLibrary_CreateWindow(PWLibrary_Engine *engine, PWLibrary_WindowInfo* info)
{
        PWLibrary_Window *nd_mbn_window = (PWLibrary_Window*) malloc(sizeof(*nd_mbn_window));
        if (nd_mbn_window == NULL)
        {
                PWLibrary_LogWarn("Warning: can not malloc window.");
                free(nd_mbn_window);
                return NULL;
        }

        size_t len = strlen(info->name);
        nd_mbn_window->name = (char*)malloc(len + 1);
        if (nd_mbn_window->name == NULL) 
        {
                free(nd_mbn_window);
                return NULL;
        }
        strcpy(nd_mbn_window->name, info->name);
                
        nd_mbn_window->window = SDL_CreateWindow(info->name, info->physic_w, info->physic_h, info->mode);
        if (nd_mbn_window->window == NULL)
        {
                PWLibrary_LogWarn("Warning: can not create window.");
                free(nd_mbn_window);
                return NULL;
        }

        SDL_ReleaseWindowFromGPUDevice(engine->device, nd_mbn_window->window);

        if (SDL_ClaimWindowForGPUDevice(engine->device, nd_mbn_window->window) == false)
        {
                PWLibrary_LogWarn("Warning: can not bind window and device.");
                free(nd_mbn_window);
                return NULL;
        }
        
        nd_mbn_window->render = SDL_CreateGPURenderer(engine->device, nd_mbn_window->window);
        if (nd_mbn_window->render == NULL)
        {
                PWLibrary_LogWarn("Warning: can not create render.");
                free(nd_mbn_window);
                return NULL;
        }
        if (SDL_SetRenderLogicalPresentation(nd_mbn_window->render, info->logical_h, info->logical_w, SDL_LOGICAL_PRESENTATION_LETTERBOX) == false)
        {
                
        }
        return nd_mbn_window;
}

void PWLibrary_DestroyWindow(PWLibrary_Window *window)
{
        if (window == NULL)
        {
                PWLibrary_LogWarn("Warning: window is null! maybe it has been destroy.");
                return;
        }

        if(window->name == NULL)
        {
                PWLibrary_LogWarn("Warning: name is null! maybe it has been destroy.");
        }
        else 
        {
                free(window->name);
        }
        
        if(window->window == NULL)
        {
                PWLibrary_LogWarn("Warning: sdl window is null! maybe it has been destroy.");
        }
        else 
        {
                SDL_DestroyWindow(window->window);
        }
        
        if(window->render)
        {
               PWLibrary_LogWarn("Warning: sdl render is null! maybe it has been destroy."); 
        }
        else 
        {
                SDL_DestroyRenderer(window->render);
        }
        
        free(window);
}

void PWLibrary_EngineRun(PWLibrary_Engine* engine)
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