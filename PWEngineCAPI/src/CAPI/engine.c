#include "PWEngineCAPI.h"
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

void PWEngineCAPI_TestEngine();

bool PWEngineCAPI_is_debug = false;

void PWEngineCAPI_setDebug()
{
        PWEngineCAPI_is_debug = true;
}

void PWEngineCAPI_LogInfo(const char *fmt)
{
        SDL_LogInfo(0, "%s", fmt);
        printf(PWE_CONSOLE_RESET "%s" PWE_CONSOLE_RESET, fmt);
}

void PWEngineCAPI_LogWarn(const char *fmt)
{
        SDL_LogWarn(0, "%s", fmt);
        SDL_LogWarn(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_YELLOW "%s" PWE_CONSOLE_RESET, fmt);
        
}

void PWEngineCAPI_LogError(const char *fmt)
{
        SDL_LogError(0, "%s", fmt);
        SDL_LogError(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_RED "%s" PWE_CONSOLE_RESET, fmt);
}

int PWEngineCAPI_ExitSDL()
{
        return 0;
}

struct PWEngineCAPI_Engine
{
        SDL_GPUDevice* device;
};

void PWEngineCAPI_PreloadInfo(SDL_GPUDevice* device)
{
        printf("PWEngine v0.0.1 beta. Copyright by riceliker and constructors. Open source license: LGPL2.0\n");
        printf("SDL Version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
        printf("Runing platform: %s\n", SDL_GetPlatform());
        printf("Graphic API: %s\n", SDL_GetGPUDeviceDriver(device));
}

PWEngineCAPI_Engine* PWEngineCAPI_CreateEngine(void)
{
        PWEngineCAPI_Engine *engine = (PWEngineCAPI_Engine*)malloc(sizeof(*engine));
        if (!TTF_Init())
        {
                PWEngineCAPI_LogError("Error: Init TTF failed.");
        }
         
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
                PWEngineCAPI_LogWarn("Error: Init SDL failed");
        } 

        SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
        engine->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

        if (engine->device == NULL)
        {
                PWEngineCAPI_LogError("Error: can not create device.");
        }

        PWEngineCAPI_PreloadInfo(engine->device);
                
        return engine;
}

void PWEngineCAPI_DestroyEngine(PWEngineCAPI_Engine* engine)
{
        if(engine == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: engine is null! destroying was stopped.");
        }
        free(engine);
}

struct PWEngineCAPI_WindowInfo
{
        char* name;
        int physic_w;
        int physic_h;
        int logical_w;
        int logical_h;
        PWEngineCAPI_WindowMode mode;
};

struct PWEngineCAPI_Window
{
        char* name;
        SDL_Window* window;
        SDL_Renderer* render;
};

PWEngineCAPI_Window* PWEngineCAPI_CreateWindow(PWEngineCAPI_Engine *engine, PWEngineCAPI_WindowInfo* info)
{
        PWEngineCAPI_Window *nd_mbn_window = (PWEngineCAPI_Window*) malloc(sizeof(*nd_mbn_window));
        if (nd_mbn_window == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: can not malloc window.");
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
                PWEngineCAPI_LogWarn("Warning: can not create window.");
                free(nd_mbn_window);
                return NULL;
        }

        SDL_ReleaseWindowFromGPUDevice(engine->device, nd_mbn_window->window);

        if (SDL_ClaimWindowForGPUDevice(engine->device, nd_mbn_window->window) == false)
        {
                PWEngineCAPI_LogWarn("Warning: can not bind window and device.");
                free(nd_mbn_window);
                return NULL;
        }
        
        nd_mbn_window->render = SDL_CreateGPURenderer(engine->device, nd_mbn_window->window);
        if (nd_mbn_window->render == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: can not create render.");
                free(nd_mbn_window);
                return NULL;
        }
        if (SDL_SetRenderLogicalPresentation(nd_mbn_window->render, info->logical_h, info->logical_w, SDL_LOGICAL_PRESENTATION_LETTERBOX) == false)
        {
                
        }
        return nd_mbn_window;
}

void PWEngineCAPI_DestroyWindow(PWEngineCAPI_Window *window)
{
        if (window == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: window is null! maybe it has been destroy.");
                return;
        }

        if(window->name == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: name is null! maybe it has been destroy.");
        }
        else 
        {
                free(window->name);
        }
        
        if(window->window == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: sdl window is null! maybe it has been destroy.");
        }
        else 
        {
                SDL_DestroyWindow(window->window);
        }
        
        if(window->render)
        {
               PWEngineCAPI_LogWarn("Warning: sdl render is null! maybe it has been destroy."); 
        }
        else 
        {
                SDL_DestroyRenderer(window->render);
        }
        
        free(window);
}

