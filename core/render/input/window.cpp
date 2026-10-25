#include "gpu.hpp"
#include "glfw/glfw3.h"

namespace PWEngine::GPU 
{
    void GPUInstance::onFocus(Window* window, int focused)
    {
        if (focused)
        {
            this->focused_window = window;
        }
        else 
        {
            if (this->focused_window == window)
                this->focused_window = nullptr;
        }
    }
}