#include "render.hpp"
#include "glfw/glfw3.h"
#include "../context/impl.hpp"

namespace PWEngine::Render 
{
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
    bool Input::checkIsMouseInput(MouseKey key)
    {
        int glfw_key = key2GLFW(key);
        return glfwGetMouseButton(this->p_context->m_window->window, glfw_key);
    }
    Utils::Vec2<float> Input::checkMousePosition()
    {
        double x, y;
        glfwGetCursorPos(this->p_context->m_window->window, &x, &y);
        return {(float)x, (float)y};
    }
    bool Input::checkIsHoverScreen()
    {
        return glfwGetWindowAttrib(this->p_context->m_window->window, GLFW_HOVERED);
    }
}