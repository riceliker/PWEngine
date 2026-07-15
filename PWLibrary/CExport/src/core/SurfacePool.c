
#include "PWL.h"
#include "PWLCollections.h"

#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"

struct PWL_Font
{
        TTF_Font* font;
};

/// =====| Surface Pool Instance |=====///
struct PWL_SurfacePool
{
        char* name;
        PWL_Dictionary* pools;
};

static void clean_surface(char* key, void* value)
{
        SDL_DestroySurface((SDL_Surface*)value);
}

PWL_SurfacePool* PWL_CreateSurfacePool(char* name)
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

/// =====| Surface Pool Element Manager |=====///
static SDL_Surface* PWL_GetSurfaceInSurfacePool(PWL_SurfacePool* self, const char* name)
{
        if (self == NULL)
        {
                PWL_LogWarn("Warning: The pool is NULL");
                return NULL;
        }
        return PWL_FindInDictionary(self->pools, name, NULL);
}

static void PWL_SetSurfaceInSurfacePool(PWL_SurfacePool* self, const char* name, SDL_Surface* surface)
{
        if (self == NULL)
        {
                PWL_LogWarn("Warning: The pool is NULL");
                return;
        }

        SDL_Surface* old_surface = (SDL_Surface*)PWL_FindInDictionary(self->pools, name, surface);
        SDL_DestroySurface(old_surface);
}

void PWL_AddSurfaceByEmptyInPool(PWL_SurfacePool* self, char* name, PWL_Vec2i size)
{
        SDL_Surface* surface = SDL_CreateSurface(size.x, size.y, SDL_PIXELFORMAT_RGBA8888);
        if (surface == NULL)
        {
                PWL_LogWarn("Warning: Create empty surface failed.");
                return;
        }
        PWL_SetSurfaceInSurfacePool(self, name, surface);
}

void PWL_AddSurfaceByIamgeInPool(PWL_SurfacePool* self, char* name, char* path)
{
        SDL_Surface* surface = IMG_Load(path);
        if (surface == NULL)
        {
                PWL_LogWarn("Warning: Create empty surface failed.");
                return;
        }
        PWL_SetSurfaceInSurfacePool(self, name, surface);
}

void PWL_AddSurfaceByTextInPool(PWL_SurfacePool* self, char* name, PWL_Font* font, int size, PWL_Color* color , char* text)
{
        SDL_Color sdl_color = {color->r, color->g, color->b, color->a};
        if (font->font == NULL)
        {
                PWL_LogWarn("Warning: Font is NULL.");
                return;
        }
        TTF_SetFontSize(font->font, size);
        SDL_Surface* surface = TTF_RenderText_Blended(font->font, text, strlen(text) , sdl_color);
        if (surface == NULL)
        {
                PWL_LogWarn("Warning: Can not create surface by text.");
                return;
        }
        PWL_SetSurfaceInSurfacePool(self, name, surface);
}

void PWL_CloneSurfaceInPool(PWL_SurfacePool* self, char* from_name, char* new_name)
{
        SDL_Surface* from_surface = PWL_GetSurfaceInSurfacePool(self, from_name);
        SDL_Surface* copy_surface = SDL_DuplicateSurface(from_surface);
        PWL_SetSurfaceInSurfacePool(self, new_name, copy_surface);
}

void PWL_CloneSurfaceFromDifferentPool(PWL_SurfacePool* from, const char* from_name, PWL_SurfacePool* to, const char* to_name)
{
        SDL_Surface* temp_ref = PWL_GetSurfaceInSurfacePool(from, from_name);
        SDL_Surface* temp = SDL_DuplicateSurface(temp_ref);
        PWL_SetSurfaceInSurfacePool(to, to_name, temp);
}

void PWL_CloneSurfaceWithScaleInPool(PWL_SurfacePool* self, char* from_name, char* new_name, float scale, PWL_SurfaceScaleMode mode)
{
        SDL_Surface* from_surface = PWL_GetSurfaceInSurfacePool(self, from_name);
        if (from_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(CloneSurfaceWithScale: surface) is null, operaction has been stopped.");
        }
        SDL_Surface* copy_surface = NULL;
        SDL_ScaleMode sdl_mode;
        switch (mode) 
        {
                case(PWL_SURFACE_SCALE_LINEAR):
                        sdl_mode = SDL_SCALEMODE_LINEAR;
                        break;
                default:
                        sdl_mode = -1;
                        break;
        }
        SDL_Rect rect = {
                0, 0, 
                (int) (from_surface->w / scale), (int) (from_surface->h / scale)
        };
        SDL_BlitSurfaceScaled(from_surface, NULL, copy_surface, &rect, sdl_mode);
        if(copy_surface == NULL)
        {
                PWL_LogWarn("Warning: use SDL_BlitSurfaceScaled create failed");
        }
        PWL_SetSurfaceInSurfacePool(self, new_name, copy_surface);
}

void PWL_CloneSurfaceWithBlitInPool(PWL_SurfacePool* self, char* from_name, char* new_name, PWL_Vec2i start, PWL_Vec2i size)
{
        SDL_Surface* from_surface = PWL_GetSurfaceInSurfacePool(self, from_name);
        if(from_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(CloneSurfaceWithBlit: surface) is null, operaction has been stopped.");
        }
        SDL_Surface* copy_surface = NULL;
        SDL_Rect rect = {
                start.x, start.y, size.x, size.y
        };
        SDL_Rect dst_rect = {
                0, 0, from_surface->w, from_surface->h
        };
        SDL_BlitSurface(from_surface, &rect, copy_surface, &dst_rect);
        if(copy_surface == NULL)
        {
                PWL_LogWarn("Warning: use SDL_BlitSurface create failed");
        }
        PWL_SetSurfaceInSurfacePool(self, new_name, copy_surface);
}
/// =====| Surface Drawing |===== ///
static uint8_t* PWL_GetPixelInSurface(PWL_SurfacePool* self, const char* name, PWL_Vec2u pos)
{
        SDL_Surface* surface = PWL_GetSurfaceInSurfacePool(self, name);
        if (surface == NULL || pos.x >= surface->w || pos.y >= surface->h)
                return NULL;
        uint8_t* pixels = (uint8_t*)surface->pixels;
        return pixels + pos.y * surface->pitch + pos.x;
}

static void PWL_DrawPixel(SDL_Surface* surface, Uint32 x, Uint32 y, Uint32 color)
{
        if (surface == NULL || x >= surface->w || y >= surface->h)
                return;
        uint8_t* pixel = (uint8_t*)surface->pixels + y * surface->pitch + x;
        *(uint8_t*)pixel = color;
}

static void PWL_DrawLineInSurface(SDL_Surface* surface, int x0, int y0, int x1, int y1, Uint32 color)
{
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
                PWL_DrawPixel(surface, x0, x0, color);
                if (x0 == x1 && y0 == y1) break;
                int e2 = err * 2;
                if (e2 > -dy) { err -= dy; x0 += sx; }
                if (e2 < dx) { err += dx; y0 += sy; }
        }
}
static void PWL_DrawRectInSurface(SDL_Surface* surface, PWL_Vec2u start, PWL_Vec2u size, Uint32 color)
{
        SDL_Rect rect = {
                start.x, start.y, size.x, size.y
        };
        
        SDL_FillSurfaceRect(surface, &rect, color);
}

static void PWL_DrawCircleInSurface(SDL_Surface* surface, PWL_Vec2u center_position, Uint32 r, Uint32 color)
{
        int cx = center_position.x;
        int cy = center_position.y;
        
        PWL_ArrayList* row = PWL_CreateArrayList();

        int x = 0;
        int y = r;
        int d = 1 - r;

        while (x <= y)
        {
                void* _x = NULL;
                PWL_New(int, x, _x);;
                PWL_FindInArrayList(row, y, _x);

                void* _y = NULL;
                PWL_New(int, y, _y)
                PWL_FindInArrayList(row, x, _y);

                x++;
                if (d < 0)
                {
                        d += 2 * x - 1;
                }
                else
                {
                        y--;
                        d += 2 * (x - y) - 1;
                }
        }

        int w0 = *(int*)PWL_FindInArrayList(row, 0, NULL);
        PWL_DrawLineInSurface(surface, cx - w0, cy, cx + w0, cy, color);

        for (int dy = 1; dy <= r; ++dy)
        {
                int w = *(int*)PWL_FindInArrayList(row, dy, NULL);
                PWL_DrawLineInSurface(surface, cx - w, cy + dy, cx + w, cy + dy, color);
                PWL_DrawLineInSurface(surface, cx - w, cy - dy, cx + w, cy - dy, color);
        }
        PWL_FreeArrayList(row);
        PWL_DestroyArrayList(row);
}

void PWL_DrawBoxInSurface(PWL_SurfacePool* self, const char* name, PWL_Vec2u position, PWL_Vec2u size, uint32_t radius, PWL_Color color)
{
        Uint32 _color = SDL_MapRGBA(SDL_GetPixelFormatDetails(
                SDL_PIXELFORMAT_RGBA8888), NULL,
                color.r, color.g, color.b, color.a
        );
        SDL_Surface* surface = PWL_GetSurfaceInSurfacePool(self, name);

        /* Standard Circle*/
        if (size.x == 0 && size.y == 0)
        {
                PWL_DrawCircleInSurface(surface, position, radius, _color);
                return;
        }

        /* Standard Rect */
        if (radius == 0)
        {
                PWL_DrawRectInSurface(surface, position, size, _color);
                return;
        }
        /* Round Rect */
        radius = PWL_Min(radius , PWL_Min(size.x/2, size.y/2));;
        PWL_Vec2u c0 =  {position.x + radius,position.y + radius};
        PWL_DrawCircleInSurface(surface, c0, radius, _color);
        PWL_Vec2u c1 =  {position.x + size.x - radius,position.y + radius};
        PWL_DrawCircleInSurface(surface, c1, radius, _color);
        PWL_Vec2u c2 =  {position.x + radius,position.y + size.y - radius};
        PWL_DrawCircleInSurface(surface, c2, radius, _color);
        PWL_Vec2u c3 =  {position.x + size.x - radius,position.y + size.y - radius};
        PWL_DrawCircleInSurface(surface, c3, radius, _color);

        PWL_Vec2u rp0 = {position.x + radius, position.y};
        PWL_Vec2u rs0 = {size.x - 2 * radius, size.y};
        PWL_DrawRectInSurface(surface, rp0, rs0, _color);

        PWL_Vec2u rp1 = {position.x, position.y + radius};
        PWL_Vec2u rs1 = {size.x, size.y  - 2 * radius};
        PWL_DrawRectInSurface(surface, rp0, rs0, _color);
}

void PWL_DrawSubsurfaceToSurfaceInPool(PWL_SurfacePool* self, char* background_surface_name, char* subsurface_name, PWL_Vec2i start, PWL_Vec2i cut_size)
{
        SDL_Surface* sdl_bg_surface = PWL_GetSurfaceInSurfacePool(self, background_surface_name);
        if(sdl_bg_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(DrawSubsurfaceInSurface: background_surface) is null, operaction has been stopped.");
                return;
        }
        SDL_Surface* sdl_sub_surface = PWL_GetSurfaceInSurfacePool(self, subsurface_name);
        if(sdl_bg_surface == NULL)
        {
                PWL_LogWarn("Warning: sdl surface(DrawSubsurfaceInSurface: subsurface) is null, operaction has been stopped.");
                return;
        }
        SDL_Rect rect = {
                start.x, start.y, cut_size.x, cut_size.y
        };
        SDL_BlitSurface(sdl_sub_surface, NULL, sdl_bg_surface, &rect);
}