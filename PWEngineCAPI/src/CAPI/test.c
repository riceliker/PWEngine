#include "PWEngineCAPI.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <stdlib.h>

void PWEngineCAPI_TestEngine_DrawText(SDL_Renderer* render, TTF_Font* font, char* text, int size, int line)
{
        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* render_triangle_number_surface =  TTF_RenderText_Blended(font, text, size, color);
        SDL_Texture* render_triangle_number_texture = SDL_CreateTextureFromSurface(render, render_triangle_number_surface);

        if(render_triangle_number_texture == NULL)
        {
                return;
        }

        SDL_FRect rect = {0, 32*line, render_triangle_number_texture->w, render_triangle_number_texture->h};

        SDL_RenderFillRect(render, &rect);

        SDL_RenderTexture(render, render_triangle_number_texture, NULL, &rect);
}

void PWEngineCAPI_TestEngineSDLRender()
{
        TTF_Init();
        SDL_Init(SDL_INIT_VIDEO);
        SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
        PWEngineCAPI_PreloadInfo(device);
        SDL_Window* window = SDL_CreateWindow("PWEngine Test Application", 1280, 720, SDL_WINDOW_RESIZABLE);
        SDL_ClaimWindowForGPUDevice(device, window);
        SDL_Renderer* render = SDL_CreateGPURenderer(device, window);
        SDL_SetRenderLogicalPresentation(render, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        TTF_Font* font = TTF_OpenFont("./Assets/jbm.ttf", 32.0);

        SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
        SDL_RenderClear(render);

        int render_triangle_number = 0;
        Uint64 render_time = SDL_GetPerformanceCounter();
        Uint64 fps_time = SDL_GetPerformanceCounter();
        Uint64 total_fps = 0;

        SDL_Event event;
        bool is_running = true;
        while (is_running) 
        {
                fps_time = SDL_GetPerformanceCounter();
                while (SDL_PollEvent(&event)) 
                {
                        SDL_Keycode key = -1;
                        switch (event.type) 
                        {
                                case SDL_EVENT_QUIT:
                                        is_running = false;
                                        break;
                                case SDL_EVENT_KEY_DOWN:
                                        key = event.key.key;
                                        break;
                                default:
                                        break;
                        }
                        switch (key) 
                        {
                                case SDLK_SPACE:
                                        SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
                                        SDL_RenderClear(render);
                                        render_triangle_number = 0;
                                        render_time = SDL_GetPerformanceCounter();
                                        Uint64 fps_time = SDL_GetPerformanceCounter();
                                        Uint64 total_fps = 0;
                                        break;
                                default:
                                        break;
                        }
                }
                
                SDL_Vertex vertices[3] = {
                        { { rand() % 1280, rand() % 720}, {1.0f, 0.0f, 0.0f, 1.0f}, {0, 0} },
                        { { rand() % 1280, rand() % 720}, {0.0f, 1.0f, 0.0f, 1.0f}, {0, 0} },
                        { { rand() % 1280, rand() % 720}, {0.0f, 0.0f, 1.0f, 1.0f}, {0, 0} },
                };
                SDL_RenderGeometry(render, NULL, vertices, 3, NULL, 0);
                
                char render_triangle_number_text[128];
                snprintf(render_triangle_number_text, 128, "TriCount:%d", render_triangle_number);
                PWEngineCAPI_TestEngine_DrawText(render, font, render_triangle_number_text, 128, 0);

                char render_time_text[128];
                snprintf(render_time_text, 128, "Time:%fs", (double)(SDL_GetPerformanceCounter() - render_time) / SDL_GetPerformanceFrequency());
                PWEngineCAPI_TestEngine_DrawText(render, font, render_time_text, 128, 1);

                char render_fps_text[128];
                total_fps += (Uint64)( 1 / ((double)(SDL_GetPerformanceCounter() - fps_time) / SDL_GetPerformanceFrequency()) );
                snprintf(render_fps_text, 128, "FPS:%0.f",total_fps / (double)render_triangle_number);
                PWEngineCAPI_TestEngine_DrawText(render, font, render_fps_text, 128, 2);

                render_triangle_number++;
                SDL_RenderPresent(render);
        }

        SDL_DestroyRenderer(render);
        SDL_DestroyWindow(window);
        SDL_DestroyGPUDevice(device);
        SDL_Quit();
}       