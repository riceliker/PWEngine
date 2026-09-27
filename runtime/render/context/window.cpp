#include "render.hpp"
#include "./render/_render.hpp"

namespace PWEngine::Render
{
    void RenderContext::hideMouseCursor()
    {
        glfwSetInputMode(this->m_window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void RenderContext::showMouseCursor()
    {
        glfwSetInputMode(this->m_window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}