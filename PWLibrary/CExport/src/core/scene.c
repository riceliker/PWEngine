#include "PWL.h"
#include "PWLCollections.h"

#include "Core.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"

struct PWL_Scene
{
        void (*Ready)(PWL_Scene* self, PWL_Event* event);
        void (*Loop)(PWL_Scene* self, PWL_Event* event, float delta);
        char* (*Leave)(PWL_Scene* self, PWL_Event* event);
        PWL_Dictionary texture_layers;
};

void PWL_SubmitSurfaceInScene(PWL_Scene* scene, PWL_SurfacePool* pool, char* name)
{

}

struct PWL_DrawSurfaceTaskInfo
{
        SDL_Texture* submit_texture;
        
}

static SDLCALL DrawSurfaceTaskOnThread(void* self)
{
        
}

char* PWL_RunScene(PWL_Scene* scene, PWL_Event* event)
{
        SDL_Event sdl_event;
        PWL_GetEventFromSDL(event, &sdl_event);
        
}

