#include "PWL.h"

#include "Core.h"
#include "PWLCollections.h"
#include "SDL3_ttf/SDL_ttf.h"

/* private */
PWL_GlobalAsset* PWL_CreateGlobalAsset()
{
        /* 0. Create Asset Instance */
        PWL_GlobalAsset* self = (PWL_GlobalAsset*)malloc(sizeof(PWL_GlobalAsset));
        if (self == NULL)
        {
                PWL_LogError("Error: Can not malloc memory for store global asset.");
                return NULL;
        }
        /* GOTO: 1 */
        /* 1. Create Font Dictionary */
        self->fonts = PWL_CreateDictionary();
        if (self->fonts == NULL)
        {
                PWL_LogError("Error: Can not create dictionary for font.");
                goto clean_assets;
        }
        /* GOTO: 2*/
        /* 2. Create Surface Dictionary */
        self->surface = PWL_CreateDictionary();
        if (self->fonts == NULL)
        {
                PWL_LogError("Error: Can not create dictionary for surface.");
                goto clean_font;
        }
        /* GOTO: 3 */

        return self;

        /* 2 */
        clean_font:
                
        /* 1 */
        clean_assets:
                free(self);
        /* End */
                return NULL;
}

/* local */
static void delete_surface_pool(char* _, void* value)
{
        PWL_SurfacePool* pool = (PWL_SurfacePool*)value;
        PWL_DestroySurfacePool(pool);
}

/* local */
static void delete_font(char* index, void* value)
{
        TTF_CloseFont((TTF_Font*)value);
}

/* private */
void PWL_DestroyGlobalAsset(PWL_GlobalAsset* self)
{
        if (self == NULL) return;
        if (self->surface != NULL)
        {
                PWL_ForeachInDictionary(self->surface, delete_surface_pool);
                PWL_DestroyDictionary(self->surface);
        }
        if (self->fonts != NULL) 
        {
                PWL_ForeachInDictionary(self->fonts, delete_font);
                PWL_DestroyDictionary(self->fonts);
        }
        free(self);
}

/* public */
void PWL_CreateFontInGlobalAsset(PWL_GlobalAsset* self, const char* name, const char* path, size_t size)
{
        TTF_Font* _font = TTF_OpenFont(path, size);
        PWL_Font* font = (PWL_Font*)malloc(sizeof(PWL_Font));
        font->font = _font;
        PWL_Font* old = PWL_FindInDictionary(self->fonts, name, font);
        TTF_CloseFont(old->font);
}

/* public */
PWL_Font* PWL_GetFontInGlobalAsset(PWL_GlobalAsset* self, const char* name)
{
        return PWL_FindInDictionary(self->fonts, name, NULL);
}

/* public */
void PWL_CreateSurfacePoolInGlobalAsset(PWL_GlobalAsset* self, const char* name)
{
        PWL_SurfacePool* pool = PWL_CreateSurfacePool(name);
        PWL_SurfacePool* old_pool = PWL_FindInDictionary(self->surface, name, pool);
        PWL_DestroySurfacePool(old_pool);
}

/* public */
PWL_SurfacePool* PWL_GetSurfacePoolInGlobalAsset(PWL_GlobalAsset* self, const char* name)
{
        return PWL_FindInDictionary(self->surface, name, NULL);
}