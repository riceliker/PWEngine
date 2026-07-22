#include "PWLCollections.h"
#include "PWL.h"

#include "Core.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"

/* local */
static void clean_surface(char* key, void* value)
{
        SDL_DestroySurface((SDL_Surface*)value);
}
/* private */
PWL_SurfacePool* PWL_CreateSurfacePool(const char* name)
{
        /* 0. Create Surface Pool */
        PWL_SurfacePool* self = (PWL_SurfacePool*)malloc(sizeof(PWL_SurfacePool));
        /* GOTO: 1 */
        /* 1. Create Surface Pool Name */
        size_t name_length = strlen(name);
        self->name = (char*)malloc(name_length * sizeof(char*));
        if (self->name == NULL)
        {
                PWL_LogWarn("Warning: Can not malloc memory for store surface pool name.");
                goto clean_surface_pool;
        }
        /* GOTO: 2 */
        strcpy(self->name, name);

        /* 2. Create pool Dictionary */
        self->pools = PWL_CreateDictionary();
        if (self->pools == NULL)
        {
                PWL_LogWarn("Warning: Can not malloc memory for store surface pool dictionary.");
                goto clean_name;
        }
        /* GOTO: 3 */
        return self;

        /* 3 */
        clen_dict:
                PWL_ForeachInDictionary(self->pools, clean_surface);
                PWL_DestroyDictionary(self->pools);
        /* 2 */
        clean_name:
                free(self->name);
        /* 1 */
        clean_surface_pool:
                free(self);
        /* End */
        return NULL;
}

/* private */
void PWL_DestroySurfacePool(PWL_SurfacePool* self)
{
        /* 3 */
        PWL_ForeachInDictionary(self->pools, clean_surface);
        PWL_DestroyDictionary(self->pools);
        /* 2 */
        free(self->name);
        /* 1 */
        free(self);
}

/* public */
PWL_Surface PWL_GetSurfaceInSurfacePool(PWL_SurfacePool* self, const char* name)
{
        PWL_Surface out = {};
        if (self == NULL)
        {
                PWL_LogWarn("Warning: The pool is NULL");
                out.surface = NULL;
                return out;
        }
        SDL_Surface* surface = PWL_FindInDictionary(self->pools, name, NULL);
        out.surface = surface;
        return out;
}

/* public */
void PWL_SetSurfaceInSurfacePool(PWL_SurfacePool* self, const char* name, PWL_Surface move_surface)
{
        if (self == NULL)
        {
                PWL_LogWarn("Warning: The pool is NULL");
                return;
        }
        SDL_Surface* old_surface = (SDL_Surface*)PWL_FindInDictionary(self->pools, name, move_surface.surface);
        SDL_DestroySurface(old_surface);
}

/* public */
void PWL_LoadSurfaceByImage(PWL_SurfacePool* self, const char* name, const char* path)
{
        if (self->status != PWL_SurfacePoolStatus_READY)
        {
                PWL_LogWarn("Warning: The surface is on sleep.");
                return;
        }
        SDL_Surface* surface = IMG_Load(path);
        if (surface == NULL)
        {
                PWL_LogWarn("Warning: Create empty surface failed.");
                return;
        }
        PWL_Surface _surface = {surface};
        PWL_SetSurfaceInSurfacePool(self, name, _surface);
}

static void load_surface_pool_from_disk(char* name, void* surface)
{
        
}

void PWL_LoadSurfacePoolFromDisk(PWL_SurfacePool* self)
{
        if (self->status != PWL_SurfacePoolStatus_SLEEP)
        {
                PWL_LogWarn("Warning: The surface pool is ready. Do not wake again.");
                return;
        }
        PWL_ForeachInDictionary(self->pools, load_surface_pool_from_disk);
}

void PWL_SaveSurfacePoolFromDisk(PWL_SurfacePool* self)
{

}