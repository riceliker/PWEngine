#ifndef PWENGINE_C_API_H
#define PWENGINE_C_API_H

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

#define PWE_CONSOLE_RESET          "\033[0m"
#define PWE_CONSOLE_RED            "\033[31m"
#define PWE_CONSOLE_GREEN          "\033[32m"
#define PWE_CONSOLE_YELLOW         "\033[33m"
#define PWE_CONSOLE_BLUE           "\033[34m"

static bool PWEngineCAPI_is_debug = false;

void PWEngineCAPI_setDebug();

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

typedef struct
{
        SDL_GPUDevice *device;
} PWEngineCAPI_Engine;

/**
 * @brief
 * Create Vulkan GPU Device.
 * If success, return 0;
 * If vulkan NOT support, return 1;
 * If SDL init failed, return 2;
 * If TTF init failed, return 3;
 * 
 * @param nd_device <SDL_GPUDevice*> [need destroy] [maybe null] new SDL_GPUDevice
 * @return <int> Error Code
 */
PWEngineCAPI_Engine* PWEngineCAPI_CreateEngine(void);

void PWEngineCAPI_DestroyEngine(PWEngineCAPI_Engine *engine);

typedef enum
{
        PWE_WINDOW_MODE_FULLSCREEN = SDL_WINDOW_FULLSCREEN,
        PWE_WINDOW_MODE_RESIZABLE = SDL_WINDOW_RESIZABLE
} PWEngineCAPI_WindowMode;

typedef struct
{
        char *name;
        int physic_w;
        int physic_h;
        int logical_w;
        int logical_h;
        PWEngineCAPI_WindowMode mode;
} PWEngineCAPI_WindowInfo;

typedef struct
{
        char *name;
        SDL_Window *window;
        SDL_Renderer *render;
} PWEngineCAPI_Window;

/**
 * @brief 
 * Create Window and binding Device and Render
 *
 * @param engine <PWEngineCAPI_Engine*> The window need depend by PWEngine instance.
 * @param info <PWEngineCAPI_WindowInfo> The info about the window.
 * @return <PWEngineCAPI_Window*> [need destroy] [maybe null] the PWEWindow Handle.
 */
PWEngineCAPI_Window* PWEngineCAPI_CreateWindow(PWEngineCAPI_Engine *engine, PWEngineCAPI_WindowInfo info);

void PWEngineCAPI_DestroyWindow(PWEngineCAPI_Window *window);


#endif