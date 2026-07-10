#include "PWL.h"
#include "collections/klib/khash.h"
#include "collections/klib/kvec.h"
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

        window->scene_map = kh_init(PWL_SceneMap_HasMap);
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

        if (window->scene_map == NULL)
        {

        }
        else 
        {
                PWL_Scene* scene;
                kh_foreach_value(window->scene_map, scene, 
                {
                        // Delete Texture Layer
                        int texture_count = scene->texture_layers.n;
                        for (int j = 0; j < texture_count; ++j)
                        {
                                free(kv_A(scene->texture_layers, j));
                        }
                        kv_destroy(scene->texture_layers);
                        // Delete Struct
                        free(scene);
                });
        }
        
        
        free(window);
}

void PWL_ShowWindow(PWL_Window* window)
{
        SDL_Event event;

        khint_t k = kh_get(PWL_SceneMap_HasMap, window->scene_map, "main");
        PWL_Scene* current_scene = kh_val(window->scene_map, k);

        int loop_mode_count = 0;
        while (true) 
        {
                /* 1.Get Event */
                if (PWL_SDLEvent(&event))
                {
                        break;
                }
                /* 2.Run Scene Loop Function */
                char scene_name[256] = "";
                current_scene->Loop(scene_name, current_scene, 1.0);
                if (strcmp(scene_name, "") != 0)
                {
                        khint_t k = kh_get(PWL_SceneMap_HasMap, window->scene_map, "main");
                        if (k == kh_end(window->scene_map))
                        {
                                PWL_LogWarn("Warning: Scene not found.");
                                goto scene_not_found; /* No.1*/
                        }
                        PWL_Scene* current_scene = kh_val(window->scene_map, k);
                }
                scene_not_found: /* Continue, If not found, warn only and run continue */
                /* 3. Set Texture */
                for (int texture_index = 0; texture_index < current_scene->texture_layers.n; ++texture_index)
                {
                        PWL_Texture* texture = kv_A(current_scene->texture_layers, texture_index);
                        if (texture->is_visible == false)
                        {
                                continue;
                        }
                        for (int i = 0; i < texture->offset_list.n; ++i)
                        {
                                if (kv_A(texture->offset_list, i) == loop_mode_count)
                                {
                                        goto texture_need_render;
                                }
                        }
                        continue;
                        texture_need_render: /* Continue, If the texture in the level. Now render it */
                        SDL_FRect texture_rect = {
                                .x = 0, .y = 0, .w = texture->texture->w, .h = texture->texture->h
                        };
                        SDL_RenderTexture(window->render, texture->texture, NULL, &texture_rect);
                }
                /* 4. Submit Texture */
                SDL_RenderPresent(window->render);
                /* 5. New Count */
                loop_mode_count = (loop_mode_count + 1) % current_scene->loop_mode;
        }
}

bool PWL_SDLEvent(SDL_Event* event)
{
        while(SDL_PollEvent(event)) 
        {
                switch (event->type) 
                {
                        case SDL_EVENT_QUIT:
                                return true;
                        default:
                                break;
                }
        }
        return false;
}