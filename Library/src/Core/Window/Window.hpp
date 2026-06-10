/*
 * Window is a root of PWEngine. When you want to start your game, you must create new PWEngine object.
 * Window support two render model: SDL_Renderer and SDL_GPU
 * Any Scene will be instance here and registry in window.
 */
#pragma once
#include "Core/Core.hpp"
#include "Core/Foundation/PWEVecMat.hpp"

#include <string>
#include <cstdlib>

#include <SDL3/SDL.h>
#include "Core/Scene/Scene.hpp"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_video.h"
#include <glm/vec2.hpp>
#include <unordered_map>
using namespace std;
using namespace glm;

namespace PWEngine::Core
{
    class PWEWindow
    {
        typedef struct
        {
            PWEVec2 logic_resolution;
            SDL_RendererLogicalPresentation presentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;
        } PWEWindowDesc;
        private: 
            SDL_Window* window;
            SDL_GPUDevice* device;
            SDL_Renderer* renderer;
            bool thread_is_running = true;
            unordered_map<string, IPWEScene*> scenes;
            vec2 window_resolution = vec2(1280, 720);
        public:
            PWEWindow(string name, PWEWindowDesc desc);
            ~PWEWindow();
            // set config
            void configWindowResolution(vec2 window_resolution){this->window_resolution=window_resolution;}
            // out info
            PWEWindowInfo getInfo(){return PWEWindowInfo {.window=this->window,.device=this->device,.renderer=this->renderer};}
            void registryScene(string unique_name, IPWEScene* scene){scenes[unique_name] = scene;}
            void runMainScene(string name);
            void event(bool is_running);
            void render(IPWEScene* scene);
    };
}