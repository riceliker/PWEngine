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

#pragma once
#include "PWMarco.h"
#include "PWLCollections.h"

/// >>>>>| Console Color |============================================================================================== ///

#define PWE_CONSOLE_RESET          "\033[0m"
#define PWE_CONSOLE_RED            "\033[31m"
#define PWE_CONSOLE_GREEN          "\033[32m"
#define PWE_CONSOLE_YELLOW         "\033[33m"
#define PWE_CONSOLE_BLUE           "\033[34m"

/// =====| Log Output |===== ///

void _PWL_LogInfo(const char* fmt, const char* file, int line)
{
        printf(PWE_CONSOLE_BLUE "[%s](%d)INFO:" PWE_CONSOLE_RESET, file, line);
        printf(PWE_CONSOLE_BLUE "%s" PWE_CONSOLE_RESET "\n", fmt);
}

void _PWL_LogWarn(const char *fmt, const char* file, int line)
{
        printf(PWE_CONSOLE_YELLOW "[%s](%d)WARN:" PWE_CONSOLE_RESET, file, line);
        printf(PWE_CONSOLE_YELLOW "%s" PWE_CONSOLE_RESET "\n", fmt);     
}

void _PWL_LogError(const char *fmt, const char* file, int line)
{
        printf(PWE_CONSOLE_RED "[%s](%d)ERROR:" PWE_CONSOLE_RESET, file, line);
        printf(PWE_CONSOLE_RED "%s" PWE_CONSOLE_RESET "\n", fmt);
        abort();
}

#define PWL_LogInfo(fmt) do{ \
_PWL_LogInfo(fmt, __FILE__, __LINE__); }while(0)

#define PWL_LogWarn(fmt) do{ \
_PWL_LogWarn(fmt, __FILE__, __LINE__); }while(0)

#define PWL_LogError(fmt) do{ \
_PWL_LogError(fmt, __FILE__, __LINE__); }while(0)

/// =====| Application Module |===== ///
PWL_STRUCT(PWL_Application)
PWL_ENUM(PWL_WindowMode)
{
        PWL_WINDOWMODE_ERROR=0,
        PWL_WINDOWMODE_FULLSCREEN=1,
        PWL_WINDOWMODE_RESIZABLE=2
};
PWL_Application* PWL_CreateApplication(char* name, PWL_Vec2i window_size, PWL_Vec2i physic_size, PWL_WindowMode mode);
void PWL_DestroyApplication(PWL_Application* self);
/// =====| Surface Pool |===== ///
PWL_STRUCT(PWL_Font)
PWL_ENUM(PWL_SurfaceScaleMode)
{
        PWL_SURFACE_SCALE_ERROR=0,
        PWL_SURFACE_SCALE_LINEAR=1
};
PWL_STRUCT(PWL_SurfacePool)
/* Surface Pool Instance */
PWL_SurfacePool* PWL_CreateSurfacePool(char* name);
void PWL_DestroySurfacePool(PWL_SurfacePool* self);
/* Surface Pool Element Manager */
void PWL_AddSurfaceByEmptyInPool(PWL_SurfacePool* self, char* name, PWL_Vec2i size);
void PWL_AddSurfaceByIamgeInPool(PWL_SurfacePool* self, char* name, char* path);
void PWL_AddSurfaceByTextInPool(PWL_SurfacePool* self, char* name, PWL_Font* font, int size, PWL_Color* color , char* text);

void PWL_CloneSurfaceInPool(PWL_SurfacePool* self, char* from_name, char* new_name);
void PWL_CloneSurfaceFromDifferentPool(PWL_SurfacePool* from, const char* from_name, PWL_SurfacePool* to, const char* to_name);
void PWL_CloneSurfaceWithScaleInPool(PWL_SurfacePool* pool, char* from_name, char* new_name, float scale, PWL_SurfaceScaleMode mode);
void PWL_CloneSurfaceWithBlitInPool(PWL_SurfacePool* pool, char* from_name, char* new_name, PWL_Vec2i start, PWL_Vec2i size);
/* Surface Drawing */
void PWL_DrawSubsurfaceToSurfaceInPool(PWL_SurfacePool* pool, char* background_surface_name, char* subsurface_name, PWL_Vec2i start, PWL_Vec2i cut_size);
void PWL_DrawBoxInSurface(PWL_SurfacePool* self, const char* name, PWL_Vec2u position, PWL_Vec2u size, uint32_t radius, PWL_Color color);
/// =====| Event Module |===== ///
PWL_STRUCT(PWL_Event)
PWL_ENUM(PWL_InputKey)
{
        PWL_KEY_NONE = -1,
        /* Number 0 - 9*/
        PWL_KEY_0 = 0,PWL_KEY_1,PWL_KEY_2,PWL_KEY_3,PWL_KEY_4,PWL_KEY_5,PWL_KEY_6,PWL_KEY_7,PWL_KEY_8,PWL_KEY_9,
        /* Alphabet 10 - 35 */
        PWL_KEY_A = 10,PWL_KEY_B,PWL_KEY_C,PWL_KEY_D,PWL_KEY_E,PWL_KEY_F,PWL_KEY_G,PWL_KEY_H,PWL_KEY_I,PWL_KEY_J,PWL_KEY_K,PWL_KEY_L,PWL_KEY_M,PWL_KEY_N,PWL_KEY_O,PWL_KEY_P,PWL_KEY_Q,PWL_KEY_R,PWL_KEY_S,PWL_KEY_T,PWL_KEY_U,PWL_KEY_V,PWL_KEY_W,PWL_KEY_X,PWL_KEY_Y,PWL_KEY_Z,
        /* Symbol 36-46*/
        PWL_KEY_MINUS=36/* - */,PWL_KEY_EQUALS/* = */,PWL_KEY_LBRACKET/* [ */,PWL_KEY_RBRACKET/* ] */,PWL_KEY_BACKSLASH/* \ */,PWL_KEY_SEMICOLON/* ; */,PWL_KEY_APOSTROPHE/* ' */,PWL_KEY_COMMA/* , */,PWL_KEY_PERIOD/* . */,PWL_KEY_SLASH/* / */,PWL_KEY_GRAVE/* ` */, 

        /* Control 100+ */
        PWL_KEY_ESCAPE = 100,PWL_KEY_ENTER,PWL_KEY_BACKSPACE,PWL_KEY_TAB,PWL_KEY_SPACE,

        /* Fuction 110+ */ 
        PWL_KEY_F1 = 110,PWL_KEY_F2,PWL_KEY_F3,PWL_KEY_F4,PWL_KEY_F5,PWL_KEY_F6,PWL_KEY_F7,PWL_KEY_F8,PWL_KEY_F9,PWL_KEY_F10,PWL_KEY_F11,PWL_KEY_F12,

        /* Direction 200+ */
        PWL_KEY_UP = 200,PWL_KEY_DOWN,PWL_KEY_LEFT,PWL_KEY_RIGHT,PWL_KEY_HOME,PWL_KEY_END,PWL_KEY_PAGEUP,PWL_KEY_PAGEDOWN,PWL_KEY_INSERT,PWL_KEY_DELETE,

        /* Mode 300+ */
        PWL_KEY_SHIFT = 300,PWL_KEY_CTRL,PWL_KEY_ALT,PWL_KEY_META
};
PWL_ENUM(PWL_InputMode)
{
        PWL_MODE_NONE=-1,PWL_MODE_SHIFT=0,PWL_MODE_CTRL,PWL_MODE_ALT,PWL_MODE_META
};
PWL_ENUM(PWL_InputMouse)
{
        PWL_MOUSE_NONE=-1,PWL_MOUSE_LEFT=0,PWL_MOUSE_MIDDLE,PWL_MOUSE_RIGHT,PWL_MOUSE_FORWARD,PWL_MOUSE_BACKWARD
};
void PWL_RegistryKeyboardEvent(PWL_Event* self, PWL_InputKey key);
/// =====| Scene Module |===== ///
PWL_STRUCT(PWL_Scene)
