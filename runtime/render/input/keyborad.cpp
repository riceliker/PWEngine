#include "render.hpp"
#include "glfw/glfw3.h"
#include "../context/impl.hpp"

namespace PWEngine::Render
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
    bool Input::checkIsKeyInput(Key key)
    {
        int glfw_key = key2GLFW(key);
        return glfwGetKey(this->p_context->m_window->window, glfw_key);
    }
}