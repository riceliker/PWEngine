#include "PWECore.hpp"

namespace PWEngine::Core
{
    void PWEEvent::event(bool is_running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT)
                is_running = false;
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_KeyboardEvent& key = event.key;
                if (!key.repeat)
                {
                    onKeyPressOnce(key.key, key.mod);
                }
                else
                {
                    onKeyHoldRepeat(key.key, key.mod);
                }
                if (event.type == SDL_EVENT_KEY_UP)
                {
                    onKeyRelease(event.key.key, event.key.mod);
                }
            }
        }
    }
    void PWEEvent::onKeyPressOnce(SDL_Keycode key, SDL_Keymod mod)
    {
        
    }
    void PWEEvent::onKeyHoldRepeat(SDL_Keycode key, SDL_Keymod mod)
    {

    }
    void PWEEvent::onKeyRelease(SDL_Keycode key, SDL_Keymod mod)
    {

    }
}