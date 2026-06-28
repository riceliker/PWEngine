#ifndef PWENGINE_C_API_H
#define PWENGINE_C_API_H

#include "PWVMT.h"

#define PWE_CONSOLE_RESET          "\033[0m\n"
#define PWE_CONSOLE_RED            "\033[31m"
#define PWE_CONSOLE_GREEN          "\033[32m"
#define PWE_CONSOLE_YELLOW         "\033[33m"
#define PWE_CONSOLE_BLUE           "\033[34m"


#define PWEngineCAPI_WindowMode int
#define PWEngineCAPI_WINDOW_MODE_FULLSCREEN 1
#define PWEngineCAPI_WINDOW_MODE_RESIZABLE 2

#define PWEngineCAPI_SurfaceScaleMode int
#define PWEngineCAPI_SURFACE_SCALE_LINEAR 1



#ifdef __cplusplus
extern "C" {
#endif


extern bool PWEngineCAPI_is_debug;

void PWEngineCAPI_setDebug();
void PWEngineCAPI_TestEngine();

void PWEngineCAPI_LogInfo(const char *info);
void PWEngineCAPI_LogWarn(const char *info);
void PWEngineCAPI_LogError(const char *info);


/**
 * @brief 
 * Clear something in the end.
 * If success, return 0;
 *
 * @return <int> Error Code
 */
int PWEngineCAPI_ExitSDL();

typedef struct PWEngineCAPI_Engine PWEngineCAPI_Engine;

/**
 * @brief
 * Create PWEngine
 * 
 * @param nd_device <SDL_GPUDevice*> [need destroy] [maybe null] new SDL_GPUDevice
 * @return <int> Error Code
 */
PWEngineCAPI_Engine* PWEngineCAPI_CreateEngine(void);

void PWEngineCAPI_DestroyEngine(PWEngineCAPI_Engine* engine);



typedef struct PWEngineCAPI_WindowInfo PWEngineCAPI_WindowInfo;

typedef struct PWEngineCAPI_Window PWEngineCAPI_Window;

/**
 * @brief 
 * Create Window and binding Device and Render
 *
 * @param engine <PWEngineCAPI_Engine*> The window need depend by PWEngine instance.
 * @param info <PWEngineCAPI_WindowInfo> The info about the window.
 * @return <PWEngineCAPI_Window*> [need destroy] [maybe null] the PWEWindow Handle.
 */
PWEngineCAPI_Window* PWEngineCAPI_CreateWindow(PWEngineCAPI_Engine *engine, PWEngineCAPI_WindowInfo* info);

void PWEngineCAPI_DestroyWindow(PWEngineCAPI_Window *window);

typedef struct PWEngineCAPI_Font PWEngineCAPI_Font;
typedef struct PWEngineCAPI_Surface PWEngineCAPI_Surface;

PWEngineCAPI_Surface* PWEngineCAPI_SurfaceBuilder(void* SDL_surface);
PWEngineCAPI_Surface* PWEngineCAPI_CreateSurfaceByEmpty(int size_x, int size_y);
PWEngineCAPI_Surface* PWEngineCAPI_CreateSurfaceByImage(char* path);
PWEngineCAPI_Surface* PWEngineCAPI_CreateSurfaceByText(PWEngineCAPI_Font* font, int size, PWColor* color , char* text);
PWEngineCAPI_Surface* PWEngineCAPI_CloneSurface(PWEngineCAPI_Surface* surface);
PWEngineCAPI_Surface* PWEngineCAPI_CloneSurfaceWithScale(PWEngineCAPI_Surface* surface, float scale, PWEngineCAPI_SurfaceScaleMode mode);
PWEngineCAPI_Surface* PWEngineCAPI_CloneSurfaceWithBlit(PWEngineCAPI_Surface* surface, PWVec2i start, PWVec2i size);


void PWEngineCAPI_DrawRectInSurface(PWEngineCAPI_Surface* surface, PWVec2i start, PWVec2i size, PWColor color);
void PWEngineCAPI_DrawSubsurfaceInSurface(PWEngineCAPI_Surface* background_surface, PWEngineCAPI_Surface* subsurface, PWVec2i start, PWVec2i cut_size);

void PWEngineCAPI_DestroySurface(PWEngineCAPI_Surface* surface);

#ifdef __cplusplus
}
#endif

#endif