#pragma once

#include "Render.hpp"

namespace PWEngine::Render 
{
    
    struct Mesh::Impl
    {
        VkBuffer vertex_buffer;
        VkDeviceMemory vertex_buffer_memory;
        VkBuffer indices_buffer;
        VkDeviceMemory indices_buffer_memory;
    };
}