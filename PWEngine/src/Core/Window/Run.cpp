#include "PWECore.hpp"

namespace PWEngine::Core
{
    void ListAllGPU()
    {
        SDL_Log("GPU Support List:");
        int gpu_count = SDL_GetNumGPUDrivers();
        for (int i = 0; i < gpu_count; i++)
        {
            const char* gpu_name = SDL_GetGPUDriver(i);
            SDL_Log("Find GPU: %s", gpu_name);
        }

    }
    PWEWindow::PWEWindow(string name, PWEWindowDesc desc)
    {
        TTF_Init();
        SDL_Init(SDL_INIT_VIDEO);
        ListAllGPU();
        // Window
        this->window = SDL_CreateWindow(name.c_str(), this->window_resolution.x, this->window_resolution.y, SDL_WINDOW_RESIZABLE);
        if (this->window == NULL) SDL_LogError(-1 , "Create window failed. From->PWEWindow:(%s) SDL->(%s)", name.c_str(), SDL_GetError());
        SDL_ShowWindow(this->window);
        SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        // GPU
        this->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,true,"vulkan");
        if (this->device == NULL) SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,true,"opengl");
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
        TTF_Quit();
    }
    void PWEWindow::runMainScene(string name)
    {
        IPWEScene* scene = this->scenes[name];
        if(scene == NULL) SDL_LogError(-1, "Scene name(%s) not found", name.c_str());
        bool is_running = true;
        while(is_running)
        {
            eventbus.event(is_running);
            render(scene);   
        }
    }
    
    void PWEWindow::render(IPWEScene* scene)
    {
        string name = scene->loop();
        if(name != "")
        {
            scene = scenes[name];
            if(scene) SDL_LogError(-1, "Scene name(%s) not found", name.c_str());
        }
    }
}