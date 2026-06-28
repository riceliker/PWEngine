#ifndef PWENGINE_C_API_H
#define PWENGINE_C_API_H

#include "PWVMT.h"

#define PWE_CONSOLE_RESET          "\033[0m\n"
#define PWE_CONSOLE_RED            "\033[31m"
#define PWE_CONSOLE_GREEN          "\033[32m"
#define PWE_CONSOLE_YELLOW         "\033[33m"
#define PWE_CONSOLE_BLUE           "\033[34m"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct PWEngineCAPI_Engine PWEngineCAPI_Engine;

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

void PWEngineCAPI_DestroyEngine(PWEngineCAPI_Engine* engine);

typedef enum PWEngineCAPI_WindowMode PWEngineCAPI_WindowMode;

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
PWEngineCAPI_Window* PWEngineCAPI_CreateWindow(PWEngineCAPI_Engine *engine, PWEngineCAPI_WindowInfo info);

void PWEngineCAPI_DestroyWindow(PWEngineCAPI_Window *window);

typedef struct PWEngineCAPI_Font PWEngineCAPI_Font;
typedef struct PWEngineCAPI_Surface PWEngineCAPI_Surface;

PWEngineCAPI_Surface* PWEngineCAPI_CreateSurfaceByEmpty(int size_x, int size_y);
PWEngineCAPI_Surface* PWEngineCAPI_CreateSurfaceByImage(char* path);
PWEngineCAPI_Surface* PWEngineCAPI_CreateSurfaceByText(PWEngineCAPI_Font* font, int size, PWVMT_Color color , char* text);
PWEngineCAPI_Surface* PWEngineCAPI_CloneSurface();

void PWEngineCAPI_DrawRectInSurface();
void PWEngineCAPI_DrawSubsurfaceInSurface();

#ifdef __cplusplus
}
#endif

#endif