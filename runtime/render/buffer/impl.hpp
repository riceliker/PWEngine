#pragma once

#include "Render.hpp"

namespace PWEngine::Render 
{
    struct VertexBuffer::Impl
    {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
    };
}