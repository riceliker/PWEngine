#include "PWEngineCAPI.h"
#include "SDL3/SDL.h"

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
                printf("SDL init error: %s\n", SDL_GetError());
                return 0;
        }

        SDL_ClearError();
        SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
        SDL_GPUDevice *test_dev = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
        if (test_dev) {
                printf("✅ Vulkan backend available\n");
                SDL_DestroyGPUDevice(test_dev);
        } else {
                printf("❌ Vulkan create failed, err: %s\n", SDL_GetError());
        }

        SDL_Quit();
    // PWEngineCAPI_Engine *engine = PWEngineCAPI_CreateEngine();

    // PWEngineCAPI_WindowInfo info = {
    //     .name = "Main", .mode = PWE_WINDOW_MODE_RESIZABLE,
    //     .physic_w = 1280, .physic_h = 720,
    //     .logical_w = 1280, .logical_h = 720,
    // };

    // PWEngineCAPI_Window *window = PWEngineCAPI_CreateWindow(engine, info);



    // PWEngineCAPI_DestroyWindow(window);

    // PWEngineCAPI_DestroyEngine(engine);
}