#include "PWL.h"

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "SDL3_ttf/SDL_ttf.h"

struct PWL_Application
{
        char* name;
        SDL_GPUDevice* device;
        SDL_Window* window;
        SDL_Renderer* render;
};

static void PWL_PreloadInfo(void* device)
{
        printf("PWEngine v0.0.1 beta. Copyright by riceliker and all contributors. Open source license: LGPL3.0\n");
        printf("SDL Version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
        printf("Runing platform: %s\n", SDL_GetPlatform());
}

PWL_Application* PWL_CreateApplication(char* name, PWL_Vec2i window_size, PWL_Vec2i logical_size, PWL_WindowMode mode)
{
        /* 0. Create Application Instance */
        PWL_Application* self = (PWL_Application*)malloc(sizeof(PWL_Application));
        if (self == NULL)
        {
                PWL_LogError("Error: Can not malloc memory for store application.");
                return NULL;
        }
        /* GOTO: 1*/
        /* Init SDL */
        if (TTF_Init() == false)
        {
                PWL_LogError("Error: Init TTF failed.");
                goto clean_application;
        }
        if(SDL_Init(SDL_INIT_VIDEO) == false)
        {
                PWL_LogError("Error: Init SDL failed");
                goto clean_application;
        } 
        SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");

        /* 1. Create SDL Device Instance */
        self->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
        if (self->device == NULL)
        {
                PWL_LogError("Error: Can not create device.");
                goto clean_application;
        }
        /* GOTO: 2 */
        PWL_PreloadInfo(self->device);

        /* 2. Create Window Name Instance */
        size_t window_name_length = strlen(name);
        self->name = (char*)calloc(window_name_length, sizeof(char*));
        if (self->name == NULL) 
        {
                PWL_LogError("Error: Can not malloc memory for store window name.");
                goto clean_device;
        }
        /* GOTO: 3 */
        strcpy(self->name, name);

        /* 3. Create SDL Window Instance */ 
        SDL_WindowFlags flag = -1;
        switch (mode) 
        {
        case PWL_WINDOWMODE_FULLSCREEN:
                flag = SDL_WINDOW_FULLSCREEN;
                break;
        case PWL_WINDOWMODE_RESIZABLE:
                flag = SDL_WINDOW_RESIZABLE;
                break;
        default:
                PWL_LogError("Error: Please input right WindowMode");
                break;
        }
        self->window = SDL_CreateWindow(self->name, window_size.x, window_size.y, flag);
        if (self->window == NULL)
        {
                PWL_LogError("Error: Can not create window.");
                goto clean_name;
        }
        /* GOTO: 4 */
        SDL_ReleaseWindowFromGPUDevice(self->device, self->window);
        if (SDL_ClaimWindowForGPUDevice(self->device, self->window) == false)
        {
                PWL_LogError("Warning: Can not bind window and device.");
                goto clean_window;
        }

        /* 4. Create SDL Render Instance*/
        self->render = SDL_CreateGPURenderer(self->device, self->window);
        if (self->render == NULL)
        {
                PWL_LogError("Warning: Can not create render.");
                goto clean_window;
        }
        /* GOTO: 5 */
        if (SDL_SetRenderLogicalPresentation(self->render, logical_size.x, logical_size.y, SDL_LOGICAL_PRESENTATION_LETTERBOX) == false)
        {
                PWL_LogError("Warning: Can not set logical resolution.");
                goto clean_render;
        }

        

        return self;
        /* 5 */
        clean_render:
                free(self->render);
        /* 4 */
        clean_window:
                free(self->window);
        /* 3 */
        clean_name:
                free(self->name);
        /* 2 */
        clean_device:
                free(self->device);
        /* 1 */
        clean_application:
                free(self);
        /* End */
        return NULL;

}

void PWL_DestroyApplication(PWL_Application* self)
{
        if (self == NULL)
                return;
        if (self->render == NULL)
                free(self->render);
        if (self->window == NULL)
                free(self->window);
        if (self->name == NULL)
                free(self->name);
        //
        if (self->device == NULL)
                free(self->device);
        /* 0 */
        free(self);
}

