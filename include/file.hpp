#pragma once
#include "stream.hpp"
#include "utils.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace PWEngine::File 
{  
    class TgaStream
    {
    private:
        std::vector<uint8_t> binary;
        std::vector<Utils::PixelRGBA8> pixels;
        struct FileInfo
        {
            uint8_t ID_length;
            uint8_t color_map_type;
            uint8_t image_type;
            uint16_t color_map_origin;
            uint16_t color_map_length;
            uint8_t color_max_depth;
            uint16_t x_origin;
            uint16_t y_origin;
            uint16_t width;
            uint16_t height;
            uint8_t pixel_depth;
            uint8_t image_descriptor;
        };
        FileInfo info;
    public:
        TgaStream(Stream::LogSystem* log, std::string load_path);
        std::unique_ptr<Utils::Image> makeImage();
    };
}