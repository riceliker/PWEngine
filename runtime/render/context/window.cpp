#include "render.hpp"
#include "impl.hpp"

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