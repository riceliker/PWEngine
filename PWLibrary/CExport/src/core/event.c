#include "PWL.h"
#include "PWLCollections.h"

#include "Core.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_scancode.h"

static PWL_InputKey TransformSDLKeyToPWLKey(SDL_Scancode code)
{
        if (code >= SDL_SCANCODE_A && code <= SDL_SCANCODE_Z)
        {
                return PWL_KEY_A + (code - SDL_SCANCODE_A);
        }
        if (code >= SDL_SCANCODE_1 && code <= SDL_SCANCODE_9)
        {
                return PWL_KEY_1 + (code - SDL_SCANCODE_1);
        }
        if (code == SDL_SCANCODE_0)
        {
                return PWL_KEY_0;
        }
        switch (code)
        {
        case SDL_SCANCODE_MINUS:        return PWL_KEY_MINUS;
        case SDL_SCANCODE_EQUALS:       return PWL_KEY_EQUALS;
        case SDL_SCANCODE_LEFTBRACKET:  return PWL_KEY_LBRACKET;
        case SDL_SCANCODE_RIGHTBRACKET:return PWL_KEY_RBRACKET;
        case SDL_SCANCODE_BACKSLASH:    return PWL_KEY_BACKSLASH;
        case SDL_SCANCODE_SEMICOLON:    return PWL_KEY_SEMICOLON;
        case SDL_SCANCODE_APOSTROPHE:   return PWL_KEY_APOSTROPHE;
        case SDL_SCANCODE_COMMA:        return PWL_KEY_COMMA;
        case SDL_SCANCODE_PERIOD:       return PWL_KEY_PERIOD;
        case SDL_SCANCODE_SLASH:        return PWL_KEY_SLASH;
        case SDL_SCANCODE_GRAVE:        return PWL_KEY_GRAVE;

        case SDL_SCANCODE_ESCAPE:       return PWL_KEY_ESCAPE;
        case SDL_SCANCODE_RETURN:       return PWL_KEY_ENTER;
        case SDL_SCANCODE_BACKSPACE:    return PWL_KEY_BACKSPACE;
        case SDL_SCANCODE_TAB:          return PWL_KEY_TAB;
        case SDL_SCANCODE_SPACE:        return PWL_KEY_SPACE;

        case SDL_SCANCODE_F1:  return PWL_KEY_F1;
        case SDL_SCANCODE_F2:  return PWL_KEY_F2;
        case SDL_SCANCODE_F3:  return PWL_KEY_F3;
        case SDL_SCANCODE_F4:  return PWL_KEY_F4;
        case SDL_SCANCODE_F5:  return PWL_KEY_F5;
        case SDL_SCANCODE_F6:  return PWL_KEY_F6;
        case SDL_SCANCODE_F7:  return PWL_KEY_F7;
        case SDL_SCANCODE_F8:  return PWL_KEY_F8;
        case SDL_SCANCODE_F9:  return PWL_KEY_F9;
        case SDL_SCANCODE_F10: return PWL_KEY_F10;
        case SDL_SCANCODE_F11: return PWL_KEY_F11;
        case SDL_SCANCODE_F12: return PWL_KEY_F12;

        case SDL_SCANCODE_UP:     return PWL_KEY_UP;
        case SDL_SCANCODE_DOWN:   return PWL_KEY_DOWN;
        case SDL_SCANCODE_LEFT:   return PWL_KEY_LEFT;
        case SDL_SCANCODE_RIGHT:  return PWL_KEY_RIGHT;
        case SDL_SCANCODE_HOME:   return PWL_KEY_HOME;
        case SDL_SCANCODE_END:    return PWL_KEY_END;
        case SDL_SCANCODE_PAGEUP: return PWL_KEY_PAGEUP;
        case SDL_SCANCODE_PAGEDOWN:return PWL_KEY_PAGEDOWN;
        case SDL_SCANCODE_INSERT: return PWL_KEY_INSERT;
        case SDL_SCANCODE_DELETE: return PWL_KEY_DELETE;

        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT: return PWL_KEY_SHIFT;
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_RCTRL:  return PWL_KEY_CTRL;
        case SDL_SCANCODE_LALT:
        case SDL_SCANCODE_RALT:   return PWL_KEY_ALT;
        case SDL_SCANCODE_LGUI:
        case SDL_SCANCODE_RGUI:   return PWL_KEY_META;

        default:
                return PWL_KEY_NONE;
        }
}

static PWL_InputMouse PWL_SDLMouseButtonToPWLMouse(int key)
{
        switch (key)
        {
        case SDL_BUTTON_LEFT:    return PWL_MOUSE_LEFT;
        case SDL_BUTTON_MIDDLE:  return PWL_MOUSE_MIDDLE;
        case SDL_BUTTON_RIGHT:   return PWL_MOUSE_RIGHT;
        case SDL_BUTTON_X1:      return PWL_MOUSE_FORWARD;
        case SDL_BUTTON_X2:      return PWL_MOUSE_BACKWARD;
        default:
                return PWL_MOUSE_NONE;
        }
}

void PWL_GetEventFromSDL(PWL_Event* self, SDL_Event* event)
{
        if (self == NULL)
        {
                PWL_LogWarn("Warning: The event is NULL");
                return;
        }
        while(SDL_PollEvent(event)) 
        {
                if (event->type == SDL_EVENT_QUIT) self->is_exit = true;
                /* Keyboard */
                self->press_key = PWL_KEY_NONE;
                self->release_key = PWL_KEY_NONE;
                if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP)
                {
                        if (event->type == SDL_EVENT_KEY_UP) 
                        {
                                self->press_key = TransformSDLKeyToPWLKey(event->key.scancode);
                        }
                        else 
                        {
                                self->release_key = TransformSDLKeyToPWLKey(event->key.scancode);
                        }
                        self->shift = self->key_down[SDL_SCANCODE_LSHIFT] || self->key_down[SDL_SCANCODE_RSHIFT];
                        self->ctrl = self->key_down[SDL_SCANCODE_LCTRL] || self->key_down[SDL_SCANCODE_RCTRL];
                        self->alt = self->key_down[SDL_SCANCODE_LALT] || self->key_down[SDL_SCANCODE_RALT];
                        self->meta = self->key_down[SDL_SCANCODE_LGUI] || self->key_down[SDL_SCANCODE_RGUI]; 
                }
                /* Mouse*/
                self->press_mouse = PWL_MOUSE_NONE;
                self->release_mouse = PWL_MOUSE_NONE;
                self->mouse_roll.x = 0;
                self->mouse_roll.y = 0;
                switch (event->type)
                {
                case SDL_EVENT_MOUSE_MOTION:
                        self->mouse_position.x = event->motion.x;
                        self->mouse_position.y = event->motion.y;
                        break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                        PWL_InputMouse button = PWL_SDLMouseButtonToPWLMouse(event->button.button);
                        if (button != PWL_MOUSE_NONE)
                        {
                                self->mouse_down[button] = true;
                                self->press_mouse = button;
                        }
                        break;
                }
                case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                        PWL_InputMouse button = PWL_SDLMouseButtonToPWLMouse(event->button.button);
                        if (button != PWL_MOUSE_NONE)
                        {
                                self->mouse_down[button] = false;
                                self->release_mouse = button;
                        }
                        break;
                }
                case SDL_EVENT_MOUSE_WHEEL:
                        self->mouse_roll.y += event->wheel.y;
                        self->mouse_roll.x += event->wheel.x;
                        break;
                }
        }
}

bool PWL_IsOnClickedFromEvent(PWL_Event* self, PWL_InputKey key, PWL_InputMode mode)
{
        if (self == NULL)
        {
                PWL_LogWarn("Warning: The event is NULL");
                return false;
        }
        switch (mode) 
        {
        case PWL_MODE_NONE:
                break;
        case PWL_MODE_ALT:
                if (self->alt == false) return false;
                break;
        case PWL_MODE_CTRL:
                if (self->ctrl == false) return false;
                break;
        case PWL_MODE_META:
                if (self->meta == false) return false;
                break;
        case PWL_MODE_SHIFT:
                if (self->meta == false) return false;
                break;
        }
        if (self->press_key == key)
                return true;
        return false;
}
/**
 * @brief 
 * Must add PWL_KEY_NONE in the end.
 */
bool PWL_IsMultiClickedFromEvent(PWL_Event* self, PWL_InputKey key, ...)
{
        if (self->press_key == key)
                return false;
        va_list ap;
        va_start(ap, trigger);
        PWL_InputKey check_key;
        while ((check_key = va_arg(ap, PWL_InputKey)) != PWL_KEY_NONE)
        {
                if (!PWL_IsOnClickedFromEvent(self, check_key, PWL_MODE_NONE))
                {
                        va_end(ap);
                        return false;
                }
        }
        va_end(ap);
        return true;
}

void PWL_InitEvent(PWL_Event* self)
{
        self->alt = false;
        self->ctrl = false;
        self->meta = false;
        self->shift = false;
        self->is_exit = false;
        self->key_down = (bool*)calloc(1024, sizeof(bool));
        self->mouse_down = (bool*)calloc(16, sizeof(bool));
        self->press_key = PWL_KEY_NONE;
        self->press_mouse = PWL_MOUSE_NONE;
        self->release_key = PWL_KEY_NONE;
        self->release_mouse = PWL_MOUSE_NONE;
        PWL_Vec2u zero_u = {0, 0};
        self->mouse_position = zero_u;
        PWL_Vec2i zero_i = {0, 0};
        self->mouse_roll = zero_i;
}

void PWL_FreeEvent(PWL_Event* self)
{
        if (self == NULL)
                return;
        free(self->key_down);
        free(self->mouse_down);
}