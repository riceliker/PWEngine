#include "Core/Scene/Scene.hpp"
#include "Core/Window/Window.hpp"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <cstddef>

namespace PWEngine::Core
{
    PWEWindow::PWEWindow(string name, PWEWindowDesc desc)
    {
        SDL_Init(SDL_INIT_VIDEO);
        // Window
        this->window = SDL_CreateWindow(name.c_str(), this->window_resolution.x, this->window_resolution.y, SDL_WINDOW_RESIZABLE);
        if (this->window == NULL) SDL_LogError(-1 , "Create window failed. From->PWEWindow:(%s) SDL->(%s)", name.c_str(), SDL_GetError());
        SDL_ShowWindow(this->window);
        SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        // GPU
        this->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,true,"vulkan");
        if (this->device == NULL) SDL_LogError(-1 , "Create device failed. From->PWEWindow:(%s) SDL->(%s)", name.c_str(), SDL_GetError());
        if(!SDL_ClaimWindowForGPUDevice(this->device, this->window)) SDL_LogError(-1, "Claimed device and window was failed. From->PWEWindow:(%s) SDL->(%s)", name.c_str(), SDL_GetError());
        // Renderer
        this->renderer = SDL_CreateRenderer(window, "vulkan,opengl,software");
        SDL_SetRenderLogicalPresentation(renderer,desc.logic_resolution.x, desc.logic_resolution.y, desc.presentation);
    }
    PWEWindow::~PWEWindow()
    {
        SDL_DestroyWindow(window);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyRenderer(renderer);
    }
    void PWEWindow::runMainScene(string name)
    {
        IPWEScene* scene = this->scenes[name];
        if(scene == NULL) SDL_LogError(-1, "Scene name(%s) not found", name.c_str());
        bool is_running = true;
        bool is_init = false;
        while(is_running)
        {
            event(is_running);
            render(scene, is_init);   
        }
    }
    void PWEWindow::event(bool is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                is_running = false;
        }
    }
    void PWEWindow::render(IPWEScene* scene, bool is_init)
    {
        if(!is_init) 
        {
            scene->init();
            is_init = true;
        }
        string name = scene->loop();
        if(name != "")
        {
            scene = scenes[name];
            if(scene) SDL_LogError(-1, "Scene name(%s) not found", name.c_str());
            is_init = true;
        }
    }
}