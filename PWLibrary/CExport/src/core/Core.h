#include "PWL.h"
#include "PWLCollections.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"


/// =====| Application Module |===== ///





/// =====| Surface |===== ///
struct PWL_Surface
{
        SDL_Surface* surface;
};
struct PWL_SurfacePool
{
        char* name;
        PWL_Dictionary* pools;
        PWL_SurfacePoolStatus status;
};

PWL_SurfacePool* PWL_CreateSurfacePool(const char* name);
void PWL_DestroySurfacePool(PWL_SurfacePool* self);



/// =====| Assets Module |===== ///
struct PWL_GlobalAsset
{
        PWL_Dictionary* fonts;
        PWL_Dictionary* surface;
};
struct PWL_Font
{
        TTF_Font* font;
};





/// =====| Event Module |===== ///
struct PWL_Event
{
        bool is_exit;
        PWL_InputKey press_key;
        PWL_InputKey release_key;
        bool* key_down;
        bool shift;
        bool ctrl;
        bool alt;
        bool meta;
        PWL_Vec2u mouse_position;
        PWL_InputMouse press_mouse;
        PWL_InputMouse release_mouse;
        bool* mouse_down;
        PWL_Vec2i mouse_roll;
};
void PWL_InitEvent(PWL_Event* self);
void PWL_FreeEvent(PWL_Event* self);
void PWL_GetEventFromSDL(PWL_Event* self, SDL_Event* event);

/// =====| Assets Module |===== ///
