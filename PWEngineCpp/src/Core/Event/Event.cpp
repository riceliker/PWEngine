#include "PWECore.hpp"
#include "SDL3/SDL_events.h"
#include <tuple>

namespace PWEngine::Core
{
    tuple<bool, SDL_Event> PWEEvent::event(bool is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if(event.type == SDL_EVENT_QUIT) 
            {
                is_running = false;
            }
        return {is_running, event};
        }
    }
    PWEInputEventPackage PWEEvent::getPackage()
    {
        return {
            on_pressed_key,
            on_pressed_mod
        };
    }

}