#include "gpu.hpp"
#include "gpu/window/_window.hpp"

namespace PWEngine::GPU
{
    static inline int key2GLFW(Key key)
    {
        const int k = static_cast<int>(key);
        const int aBegin = static_cast<int>(Key::A);
        const int zEnd = static_cast<int>(Key::Z);
        if (k >= aBegin && k <= zEnd)
        {
            return 65 + (k - aBegin);
        }
        const int num0Begin = static_cast<int>(Key::Num0);
        const int num9End = static_cast<int>(Key::Num9);
        if (k >= num0Begin && k <= num9End)
        {
            return 48 + (k - num0Begin);
        }
        const int kp0Begin = static_cast<int>(Key::Kp0);
        const int kp9End = static_cast<int>(Key::Kp9);
        if (k >= kp0Begin && k <= kp9End)
        {
            return 320 + (k - kp0Begin);
        }
        switch (key)
        {
            case Key::Space:      return GLFW_KEY_SPACE;
            case Key::Enter:      return GLFW_KEY_ENTER;
            case Key::Tab:        return GLFW_KEY_TAB;
            case Key::Backspace:  return GLFW_KEY_BACKSPACE;

            case Key::Left:   return GLFW_KEY_LEFT;
            case Key::Right:  return GLFW_KEY_RIGHT;
            case Key::Up:     return GLFW_KEY_UP;
            case Key::Down:   return GLFW_KEY_DOWN;

            case Key::Insert:     return GLFW_KEY_INSERT;
            case Key::Delete:     return GLFW_KEY_DELETE;
            case Key::Home:       return GLFW_KEY_HOME;
            case Key::End:        return GLFW_KEY_END;
            case Key::PageUp:     return GLFW_KEY_PAGE_UP;
            case Key::PageDown:   return GLFW_KEY_PAGE_DOWN;

            case Key::F1:  return GLFW_KEY_F1;
            case Key::F2:  return GLFW_KEY_F2;
            case Key::F3:  return GLFW_KEY_F3;
            case Key::F4:  return GLFW_KEY_F4;
            case Key::F5:  return GLFW_KEY_F5;
            case Key::F6:  return GLFW_KEY_F6;
            case Key::F7:  return GLFW_KEY_F7;
            case Key::F8:  return GLFW_KEY_F8;
            case Key::F9:  return GLFW_KEY_F9;
            case Key::F10: return GLFW_KEY_F10;
            case Key::F11: return GLFW_KEY_F11;
            case Key::F12: return GLFW_KEY_F12;

            case Key::KpDecimal:  return GLFW_KEY_KP_DECIMAL;
            case Key::KpDivide:   return GLFW_KEY_KP_DIVIDE;
            case Key::KpMultiply: return GLFW_KEY_KP_MULTIPLY;
            case Key::KpSubtract: return GLFW_KEY_KP_SUBTRACT;
            case Key::KpAdd:      return GLFW_KEY_KP_ADD;
            case Key::KpEnter:    return GLFW_KEY_KP_ENTER;

            case Key::LShift: return GLFW_KEY_LEFT_SHIFT;
            case Key::RShift: return GLFW_KEY_RIGHT_SHIFT;
            case Key::LCtrl:  return GLFW_KEY_LEFT_CONTROL;
            case Key::RCtrl:  return GLFW_KEY_RIGHT_CONTROL;
            case Key::LAlt:   return GLFW_KEY_LEFT_ALT;
            case Key::RAlt:   return GLFW_KEY_RIGHT_ALT;
            case Key::LSuper: return GLFW_KEY_LEFT_SUPER;
            case Key::RSuper: return GLFW_KEY_RIGHT_SUPER;
            case Key::Menu:   return GLFW_KEY_MENU;

            case Key::Escape: return GLFW_KEY_ESCAPE;

            default:
                return GLFW_KEY_UNKNOWN;
        }
    }

    static inline int key2GLFW(MouseKey key)
    {
        switch (key) 
        {
        case BtnLeft: return GLFW_MOUSE_BUTTON_LEFT;
        case BtnRight: return GLFW_MOUSE_BUTTON_RIGHT;
        case BtnMiddle: return GLFW_MOUSE_BUTTON_MIDDLE;
        default: return -1;
        }
    }

    void GPUInstance::updateInput()
    {
        uint8_t key_index = 0;
        while (static_cast<Key>(key_index) != Key::EndOfEnum) 
        {
            this->input.key_list[key_index] = glfwGetKey(this->focused_window->self->window, key2GLFW(static_cast<Key>(key_index)));
            ++key_index;    
        }

        uint8_t mouse_index = 0;
        while (static_cast<MouseKey>(mouse_index) != MouseKey::EndOfEnum) 
        {
            this->input.mouse_key[mouse_index] = glfwGetKey(this->focused_window->self->window, key2GLFW(static_cast<MouseKey>(mouse_index)));
            ++mouse_index;    
        }

        double x, y;
        glfwGetCursorPos(this->focused_window->self->window, &x, &y);
        this->input.mouse_position_x = x;
        this->input.mouse_position_y = y;

        this->input.is_hover =  glfwGetWindowAttrib(this->focused_window->self->window, GLFW_HOVERED);

        this->input.window_should_close = glfwWindowShouldClose(this->focused_window->self->window);
    }
    
}
