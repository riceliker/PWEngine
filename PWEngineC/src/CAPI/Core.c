#include "PWEngineCAPI.h"
#include "PWVMT.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_video.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <stdlib.h>

void PWEngineCAPI_setDebug()
{
        PWEngineCAPI_is_debug = true;
}

void PWEngineCAPI_LogInfo(const char *fmt)
{
        SDL_LogInfo(0, "%s", fmt);
        printf(PWE_CONSOLE_RESET "%s\n" PWE_CONSOLE_RESET, fmt);
}

void PWEngineCAPI_LogWarn(const char *fmt)
{
        SDL_LogWarn(0, "%s", fmt);
        SDL_LogWarn(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_YELLOW "%s\n" PWE_CONSOLE_RESET, fmt);
        
}

void PWEngineCAPI_LogError(const char *fmt)
{
        SDL_LogError(0, "%s", fmt);
        SDL_LogError(0, "%s", SDL_GetError());
        printf(PWE_CONSOLE_RED "%s\n" PWE_CONSOLE_RESET, fmt);
}

int PWEngineCAPI_ExitSDL()
{
        return 0;
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

        engine->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");

        if (engine->device == NULL)
        {
                PWEngineCAPI_LogError("Error: can not create device.");
        }
                
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

PWEngineCAPI_Window* PWEngineCAPI_CreateWindow(PWEngineCAPI_Engine *engine, PWEngineCAPI_WindowInfo info)
{
        PWEngineCAPI_Window *nd_mbn_window = (PWEngineCAPI_Window*) malloc(sizeof(*nd_mbn_window));
        if (!nd_mbn_window)
        {
                PWEngineCAPI_LogWarn("Warning: can not malloc window.");
                return NULL;
        }

        size_t len = strlen(info.name);
        nd_mbn_window->name = (char*)malloc(len + 1);
        if (!nd_mbn_window->name) {
                free(nd_mbn_window);
                return NULL;
        }
        strcpy(nd_mbn_window->name, info.name);
                
        nd_mbn_window->window = SDL_CreateWindow(info.name, info.physic_w, info.physic_h, info.mode);
        if (nd_mbn_window->window == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: can not create window.");
                return NULL;
        }
        if (SDL_ClaimWindowForGPUDevice(engine->device, nd_mbn_window->window))
        {
                PWEngineCAPI_LogWarn("Warning: can not bind window and device.");
                return NULL;
        }
        
        nd_mbn_window->render = SDL_CreateGPURenderer(engine->device, nd_mbn_window->window);
        if (nd_mbn_window->render)
        {
                PWEngineCAPI_LogWarn("Warning: can not create render.");
                return NULL;
        }
        if (SDL_SetRenderLogicalPresentation(nd_mbn_window->render, info.logical_h, info.logical_w, SDL_LOGICAL_PRESENTATION_LETTERBOX))
        {
                
        }
        return 0;
}

void PWEngineCAPI_DestroyWindow(PWEngineCAPI_Window *window)
{
        if (window == NULL)
        {
                PWEngineCAPI_LogWarn("Warning: window is null! destroying was stopped.");
        }
        free(window->name);
        free(window);
}
