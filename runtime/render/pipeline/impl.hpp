#pragma once
#include "render.hpp"

namespace PWEngine::Render 
{
    
    struct Texture::Impl
    {
        Utils::Vec2<size_t> size;
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        VkImage texture_image;
        VkDeviceMemory texture_imageMemory;
    };
}