#include "PWL.h"
#include "SDL3/SDL_stdinc.h"
#include "collections/klib/kvec.h"
#include "core/core.h"

#include <stdlib.h>

PWL_Scene* PWL_RegistryScene(const PWL_Window* window, const char* scene_name, Uint16 loop_mode , void (*Loop)(char* next_scene, PWL_Scene* scene, float delta))
{
        PWL_Scene* scene = (PWL_Scene*)malloc(sizeof(PWL_Scene));
        scene->render = window->render;
        kv_init(scene->texture_layers);
        scene->Loop = Loop;
        scene->loop_mode = loop_mode;
        int* r;
        kh_put(PWL_SceneMap_HasMap, window->scene_map, scene_name, r);
        switch (*r) 
        {
                case 0:

        }
        khint_t k = kh_get(PWL_SceneMap_HasMap, window->scene_map, scene_name);
        PWL_Scene* surface = NULL;
        if (k != kh_end(window->scene_map))
        {
                surface = kh_val(window->scene_map, k);
        }
        return scene;
}

void PWL_RegistryTextureInSceneLoop(PWL_Scene* scene, PWL_Texture* texture)
{
        if (scene == NULL)
        {
                PWL_LogWarn("Warning: The scene is null.");
                return;
        }
        kv_push(PWL_Texture*, scene->texture_layers, texture);
}
