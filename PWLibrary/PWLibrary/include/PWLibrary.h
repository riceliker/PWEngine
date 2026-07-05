/**
 * @file PWLibrary.h
 * @author riceliker and all constructors (github: https://github.com/riceliker)
 * @brief 
 * @version 0.1.0 beta
 * @date 2026-07-05
 * 
 * @copyright Copyright (c) 2026 by riceliker and constructors. Open source license: LGPL3.0
 * 
 * Notice: use header file to private project is allowed, but you can NOT change this file without open source.
 */
#pragma once

#ifndef PWENGINE_C_API_H
#define PWENGINE_C_API_H

#include "PWVMT.h"

/// ===== Console Color ===== ///

#define PWE_CONSOLE_RESET          "\033[0m\n"
#define PWE_CONSOLE_RED            "\033[31m"
#define PWE_CONSOLE_GREEN          "\033[32m"
#define PWE_CONSOLE_YELLOW         "\033[33m"
#define PWE_CONSOLE_BLUE           "\033[34m"

/// ===== Enum(both C and C++) ===== ///

#define PWLibrary_WindowMode int
#define PWLibrary_WINDOWMODE_FULLSCREEN 1
#define PWLibrary_WINDOWMODE_RESIZABLE 2

#define PWLibrary_SurfaceScaleMode int
#define PWLibrary_SURFACE_SCALE_LINEAR 1

#define PWLibrary_ShaderType int
#define PWLibrary_SHADER_TYPE_VERTEX 1
#define PWLibrary_SHADER_TYPE_FRAGMENT 2
#define PWLibrary_SHADER_TYPE_COMPUTE 3

// for C++

#ifdef __cplusplus
extern "C" {
#endif

/// ===== Log Module ===== ///

/**
 * The Log System of PWLibrary, it will print the log to console.
 * Type <Info> : Only tell the status of the engine.
 * Type <Warn> : Engine can be run, but some actions are abnormal.
 * Type <Error> : Engine can not run, and will exit process now.
 * Type <Debug> : Only show something on debug mode.
 */
void PWLibrary_LogInfo(const char *info);
void PWLibrary_LogWarn(const char *info);
void PWLibrary_LogError(const char *info);

/**
 * Show some basic info in console, It will help you debug easily.
 */
void PWLibrary_PreloadInfo(void* device);

/// ===== Engine Module ===== ///

/**
 * @brief 
 * The struct is the main engine of PWEngine, it will store the SDL GPU Device and Engine EventBus.
 */
typedef struct PWLibrary_Engine PWLibrary_Engine;

/**
 * @brief
 * Create PWEngine, which include the SDL GPU Device and SDL EventBus
 *  
 * @return <PWLibrary_Engine*> [need destroy] [maybe null] new Engine Instance
 */
PWLibrary_Engine* PWLibrary_CreateEngine(void);

void PWLibrary_EngineRun(PWLibrary_Engine* engine);

/**
 * @brief 
 * Destroy PWEngine, it will check the ptr is null.
 * 
 * @param engine description
 * @param engine 
 */
void PWLibrary_DestroyEngine(PWLibrary_Engine* engine);

/// ===== Window Module ===== ///

/**
 * @brief 
 * The struct is make the config for function CreateWindow.
 * @param char* name: Window title name.
 * @param int physic_w: Physic window width
 * @param int physic_h: Physic window height
 * @param int logical_w: Logical window width
 * @param int logical_h: Logical window height
 * @param PWLibrary_WindowMode mode: Window mode
 */
typedef struct PWLibrary_WindowInfo PWLibrary_WindowInfo;

/**
 * @brief
 * The struct is store SDL_Window and SDL_Renderer. 
 * @param char* name: Window name
 * @param SDL_Window* window: SDL_Window Instance
 * @param SDL_Renderer* render: SDL_Renderer Instance
 */
typedef struct PWLibrary_Window PWLibrary_Window;

/**
 * @brief 
 * Create Window and binding Device and Render
 *
 * @param engine <PWLibrary_Engine*> The window need depend by PWEngine instance.
 * @param info <PWLibrary_WindowInfo> The info about the window.
 * @return <PWLibrary_Window*> [need destroy] [maybe null] the PWEWindow Handle.
 */
PWLibrary_Window* PWLibrary_CreateWindow(PWLibrary_Engine *engine, PWLibrary_WindowInfo* info);

void PWLibrary_DestroyWindow(PWLibrary_Window *window);

/// ===== Surface Module ===== ///

typedef struct PWLibrary_Font PWLibrary_Font;
typedef struct PWLibrary_Surface PWLibrary_Surface;

PWLibrary_Surface* PWLibrary_SurfaceBuilder(void* SDL_surface);
PWLibrary_Surface* PWLibrary_CreateSurfaceByEmpty(int size_x, int size_y);
PWLibrary_Surface* PWLibrary_CreateSurfaceByImage(char* path);
PWLibrary_Surface* PWLibrary_CreateSurfaceByText(PWLibrary_Font* font, int size, PWColor* color , char* text);
PWLibrary_Surface* PWLibrary_CloneSurface(PWLibrary_Surface* surface);
PWLibrary_Surface* PWLibrary_CloneSurfaceWithScale(PWLibrary_Surface* surface, float scale, PWLibrary_SurfaceScaleMode mode);
PWLibrary_Surface* PWLibrary_CloneSurfaceWithBlit(PWLibrary_Surface* surface, PWVec2i start, PWVec2i size);


void PWLibrary_DrawRectInSurface(PWLibrary_Surface* surface, PWVec2i start, PWVec2i size, PWColor color);
void PWLibrary_DrawSubsurfaceInSurface(PWLibrary_Surface* background_surface, PWLibrary_Surface* subsurface, PWVec2i start, PWVec2i cut_size);

void PWLibrary_DestroySurface(PWLibrary_Surface* surface);




#ifdef __cplusplus
}
#endif

#endif