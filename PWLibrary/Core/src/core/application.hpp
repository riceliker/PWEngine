#pragma once
#include "public/core.hpp"

#include "core/log.hpp"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>

#include "SDL3_ttf/SDL_ttf.h"


using namespace std;
using namespace PWL::Log;
namespace PWL::Core {

    inline void Init()
    {
        if (TTF_Init() == false)
        {
            LogError("The SDL3 TTF Module can not be init. Try to reinstall SDL3_ttf Library.");
            abort();
        }
        if (SDL_Init(SDL_INIT_VIDEO) == false)
        {
            LogError("The SDL3 Module can not be init. Try to reinstall SDL3 Library.");
            abort();
        }
        SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");

        printf("PWLibrary v0.0.1 beta. Copyright by riceliker and all contributors. Open source license: LGPL3.0\n");
        printf("SDL Version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
        printf("Runing platform: %s\n", SDL_GetPlatform());
    }

    struct Application::Impl
    {
        SDL_GPUDevice* device;
        SDL_Window* window;
        SDL_Renderer* render;
    };
    Application::~Application()
    {
        SDL_DestroyRenderer(this->impl->render);
        SDL_DestroyWindow(this->impl->window);
        SDL_DestroyGPUDevice(this->impl->device);
    }
    optional<shared_ptr<Application>> Application::CreateApplication(string name, Vec2<uint32_t> physics_resolution, Vec2<uint32_t> logical_resolution, ApplicationWindowMode mode)
    {
        shared_ptr<Application> self(new Application());

        SDL_GPUDevice* device = nullptr;
        SDL_Window* window = nullptr;
        SDL_Renderer* render = nullptr;

        SDL_WindowFlags flag = -1;
        /* Device */
        device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
        if (device == NULL)
        {
            LogError("Can not create device from SDL3.");
            goto fail;
        }

        /* Window */
        switch (mode) 
        {
        case PWL_WINDOWMODE_FULLSCREEN:
            flag = SDL_WINDOW_FULLSCREEN;
            break;
        case PWL_WINDOWMODE_RESIZABLE:
            flag = SDL_WINDOW_RESIZABLE;
            break;
        default:
            LogError("Error: Please input right WindowMode");
            goto clean_device;
        }
        window = SDL_CreateWindow(name.c_str(), physics_resolution.x, physics_resolution.y, flag);
        if (window == NULL)
        {
            LogError("Can not create window from SDL3.");
            goto clean_device;
        }

        /* Bind Device & Window */
        if (SDL_ClaimWindowForGPUDevice(device, window) == false)
        {
            LogError("Can not bind window and device.");
            goto clean_window;
        }

        /* Render */
        render = SDL_CreateGPURenderer(device, window);
        if (render == NULL)
        {
            LogError("Can not create Render from SDL3.");
            goto clean_window;
        }
        if (SDL_SetRenderLogicalPresentation(render, logical_resolution.x, logical_resolution.y, SDL_LOGICAL_PRESENTATION_LETTERBOX) == false)
        {
            LogError("Can not set logical resolution.");
            goto clean_render;
        }

        /* build */
        self->impl->device = device;
        self->impl->window = window;
        self->impl->render = render;

        return self;

        clean_render:
            SDL_DestroyRenderer(render);
        clean_window:
            SDL_DestroyWindow(window);
        clean_device:
            SDL_DestroyGPUDevice(device);
        fail:
            return nullopt;
    }

    
}

