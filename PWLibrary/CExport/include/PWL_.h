/**
 * @file PWL.h
 * @author riceliker and all constructors (github: https://github.com/riceliker)
 * @brief 
 * @version 0.1.0 beta
 * @date 2026-07-05
 * 
 * @copyright Copyright (c) 2026 by riceliker and all constructors. Open source license: LGPL3.0
 * 
 * Notice: Use header file to private project is allowed, but you can NOT modify this file without open source.
 * Dependent Header: PW_wmt.h
 */

/// =======================================================================================================================
/**
        This yield will tell something basic code to help you use when you forget something detail.
        But it is not a course for PWLibrary, if you want to know how to learn PWLibrary, See Document.
*/
/// =======================================================================================================================

#pragma once

#include "PW_vmt.h"
#include "PWMarco.h"

/// >>>>>| Global Macro |=============================================================================================== ///

/* Extern C macro template */
#ifdef __cplusplus
#  define PW_EXTERN_C_BEGIN extern "C" {
#  define PW_EXTERN_C_END   }
#else
#  define PW_EXTERN_C_BEGIN
#  define PW_EXTERN_C_END
#endif

/* Public struct define */
#ifdef __cplusplus
#  define PublicDefineStruct(Name) struct Name;
#else
#  define PublicDefineStruct(Name) typedef struct Name Name; struct Name;
#endif

/* Public enum define */
#ifdef __cplusplus
#  define PublicDefineEnum(Name) enum Name
#else
#  define PublicDefineEnum(Name) typedef enum Name Name; enum Name
#endif

/// >>>>>| Enum(both C and C++) |======================================================================================= ///

#define PWL_WindowMode int
#define PWL_WINDOWMODE_ERROR 0
#define PWL_WINDOWMODE_FULLSCREEN 1
#define PWL_WINDOWMODE_RESIZABLE 2

#define PWL_SurfaceScaleMode int
#define PWL_SURFACE_SCALE_ERROR 0
#define PWL_SURFACE_SCALE_LINEAR 1


/// >>>>>| The C++ can read this header file directly |================================================================= ///

#ifdef __cplusplus
extern "C" {
#endif

/// >>>>>| Log Module |================================================================================================== ///

/**
 * The Log System of PWL, it will print the log to console.
 * Type <Info> : Only tell the status of the engine.
 * Type <Warn> : Engine can be run, but some actions are abnormal.
 * Type <Error> : Engine can not run, and will exit process now.
 * Type <Debug> : Only show something on debug mode.
 */
void PWL_LogInfo(const char *info);
void PWL_LogWarn(const char *info);
void PWL_LogError(const char *info);

/**
 * Show some basic info in console, It will help you debug easily.
 */
void PWL_PreloadInfo(void* device);

/// =====| Application Module |===== ///

PWL_STRUCT(PWL_Application)


/// >>>>>| Util Module |================================================================================================= ///


/// >>>>>| Font Module |================================================================================================= ///

typedef struct PWL_Font PWL_Font;

/// >>>>>| Surface Module |============================================================================================== ///
typedef struct PWL_Surface PWL_Surface;
typedef struct PWL_SurfacePool PWL_SurfacePool;

PWL_SurfacePool* PWL_CreateSurfacePool(void);
void PWL_DestroySurfacePool(PWL_SurfacePool* pool);

void PWL_AddSurfaceByEmptyInPool(PWL_SurfacePool* pool, char* name, PW_vmt_Vec2i size);
void PWL_AddSurfaceByIamgeInPool(PWL_SurfacePool* pool, char* name,  char* path);
void PWL_AddSurfaceByTextInPool(PWL_SurfacePool* pool, char* name, PWL_Font* font, int size, PW_vmt_Color* color , char* text);
void PWL_AddSurfaceByCopySurfaceInPool(PWL_SurfacePool* pool, char* name, PWL_Surface surface);
PWL_Surface PWL_ExportSurfaceByCopySurfaceInPool(PWL_SurfacePool* pool, char* name);

void PWL_CloneSurfaceInPool(PWL_SurfacePool* pool, char* from_name, char* new_name);
void PWL_CloneSurfaceWithScaleInPool(PWL_SurfacePool* pool, char* from_name, char* new_name, float scale, PWL_SurfaceScaleMode mode);
void PWL_CloneSurfaceWithBlitInPool(PWL_SurfacePool* pool, char* from_name, char* new_name, PW_vmt_Vec2i start, PW_vmt_Vec2i size);

void PWL_DrawRectToSurfaceInPool(PWL_SurfacePool* pool, char* name, PW_vmt_Vec2i start, PW_vmt_Vec2i size, PW_vmt_Color color);
void PWL_DrawSubsurfaceToSurfaceInPool(PWL_SurfacePool* pool, char* background_surface_name, char* subsurface_name, PW_vmt_Vec2i start, PW_vmt_Vec2i cut_size);



/// >>>>>| Engine Module |=============================================================================================== ///

/**
 * @brief 
 * The struct is the main engine of PWEngine, it will store the SDL GPU Device and Engine EventBus.
 */
typedef struct PWL_Engine PWL_Engine;

/**
 * @brief
 * Create PWEngine, which include the Render and EventBus
 *  
 * @return <PWL_Engine*> [need destroy] [maybe null] new Engine Instance
 */
PWL_Engine* PWL_CreateEngine(void);

/**
 * @brief 
 * The Engine must destroy by this function.
 * 
 * @param engine description
 * @param engine 
 */
void PWL_DestroyEngine(PWL_Engine* engine);


/// >>>>>| Window Module |=============================================================================================== ///

/**
 * @brief 
 * The struct is make the config for function CreateWindow.
 * @param char* name: Window title name.
 * @param int physic_w: Physic window width
 * @param int physic_h: Physic window height
 * @param int logical_w: Logical window width
 * @param int logical_h: Logical window height
 * @param PWL_WindowMode mode: Window mode
 */
typedef struct PWL_WindowInfo PWL_WindowInfo;

/**
 * @brief
 * The struct is store SDL_Window and SDL_Renderer. 
 * @param char* name: Window name
 * @param SDL_Window* window: SDL_Window Instance
 * @param SDL_Renderer* render: SDL_Renderer Instance
 */
typedef struct PWL_Window PWL_Window;

/**
 * @brief 
 * Create Window and binding Device and Render
 *
 * @param engine <PWL_Engine*> The window need depend by PWEngine instance.
 * @param info <PWL_WindowInfo> The info about the window.
 * @return <PWL_Window*> [need destroy] [maybe null] the PWEWindow Handle.
 */
PWL_Window* PWL_CreateWindow(PWL_Engine *engine, PWL_WindowInfo* info);
void PWL_DestroyWindow(PWL_Window *window);

void PWL_ShowWindow(PWL_Window* window);

/// >>>>>| Scene Module |=============================================================================================== ///
/// Document ----------------------------------------------------------------------------------------------------------- ///
//|     
//|     1.Define the callback function
//|     void ExampleSceneLoop(char* next_scene, PWL_Scene* scene, float delta)
//|     {
//|             /* Define a Texture */
//|             PWL_Texture* texture = ...
//|             PWL_RegistryTextureInSceneLoop(, texture);
//|             /* Set Next Scene */
//|             if (is_back)
//|                     next_scene = "home";
//|             else:
//|                     next_scene = ""; /* Mean not change scene */
//|     }
//|     2.Submit the 
/// --------------------------------------------------------------------------------------------------------------------- ///
/**
 * @brief
 * The struct is store the SDL_Texture. 
 */
typedef struct PWL_Texture PWL_Texture;
typedef struct PWL_Scene PWL_Scene;
typedef struct PWL_SceneLoopPackage PWL_SceneLoopPackage;

/**
 * @brief 
 * Create scene and registry in window, then it can be switch scene by name.
 * It will return the PWL_Scene, you can use this handle to init scene.
 * @param window <const * _> <PWL_Window*> the window handle
 * @param scene_name <const * _> <char*> the scene name
 */
void PWL_RegistrySceneToWindow(const PWL_Window* window, const char* scene_name,
        void (*Loop)(char* next_scene, PWL_Scene* scene, float delta)
);

void PWL_RegistryTextureInSceneLoop(PWL_Scene* scene, PWL_Texture* texture);


#ifdef __cplusplus
}
#endif
