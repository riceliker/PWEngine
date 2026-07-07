#include "PWL.h"
#include "core/core.h"

PWL_Scene* PWL_CreateScene(PWL_Window* window)
{
        PWL_Scene* scene = (PWL_Scene*)malloc(sizeof(PWL_Scene));
        window->render = scene->not_delete_render;
        scene->texture_levels = 0;
        scene->Loop = ExampleEmptyLoop;
}

void PWL_DestroyScene(PWL_Scene* scene)
{
        
}

void PWL_SetLoopFunctionInScene()
{

}