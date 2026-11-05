#include "file.hpp"
#include "file/image/_bitreader.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace PWEngine::File 
{ 
    ImageRGBA8::ImageRGBA8(TGA, LogSystem* log, std::string path)
    {
        auto _file = readBinaryFile(log, path);
        if (!_file.has_value()) {
            sendLog(log, LogType::Warn, LogFrom::FileIO, "Can not open file! From:" + path);
            return;
        }
        auto file = _file.value();
        if (file.size() < 18) {
            sendLog(log, LogType::Warn, LogFrom::FileIO, "The file is broken! From:" + path);
            return;
        }
        auto ptr = file.data();
        uint8_t id_length = get8(ptr);
        uint8_t color_map_type = get8(ptr);
        uint8_t image_type = get8(ptr);
        uint16_t color_map_origin = get16le(ptr);
        uint16_t color_map_length = get16le(ptr);
        uint8_t color_max_depth = get8(ptr);
        uint16_t x_origin = get16le(ptr);
        uint16_t y_origin = get16le(ptr);
        uint16_t width  = get16le(ptr);
        uint16_t height = get16le(ptr);
        uint8_t pixel_depth = get8(ptr);
        uint8_t image_descriptor = get8(ptr);

        bool start_direct = (image_descriptor & (1u << 5)) != 0;

        if (color_map_type == 1)
        {
            sendLog(log, LogType::Warn, LogFrom::FileIO, "Not support the color map! From:" + path);
        }
        else 
        {
            std::vector<Vec4<uint8_t>> pixels;
            switch (pixel_depth) 
            {
            case 8:
                sendLog(log, LogType::Warn, LogFrom::FileIO, "Not support the gray image! From:" + path);
            return;
            case 16:
                sendLog(log, LogType::Warn, LogFrom::FileIO, "Not support the RGB555 image! From:" + path);
            return;
            case 24:
                if (start_direct) {
                    for (size_t i = 0; i < width * height; ++i) {
                        uint8_t b = get8(ptr);
                        uint8_t g = get8(ptr);
                        uint8_t r = get8(ptr);
                        uint8_t a = 255;
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                    }
                } else {
                    ptr += width * height - 4;
                    for (size_t i = 0; i < width * height; ++i) {
                        uint8_t b = get8m(ptr);
                        uint8_t g = get8m(ptr);
                        uint8_t r = get8m(ptr);
                        uint8_t a = 255;
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                    }
                }

            break;
            case 32:
                if (start_direct) {
                    for (size_t i = 0; i < width * height; ++i) {
                        uint8_t b = get8(ptr);
                        uint8_t g = get8(ptr);
                        uint8_t r = get8(ptr);
                        uint8_t a = get8(ptr);
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                    }
                } else {
                    ptr += width * height - 4;
                    for (size_t i = 0; i < width * height; ++i) {
                        uint8_t b = get8m(ptr);
                        uint8_t g = get8m(ptr);
                        uint8_t r = get8m(ptr);
                        uint8_t a = get8(ptr);
                        pixels.emplace_back(Vec4<uint8_t>{r, g, b, a});
                    }
                }
            break;
            default:
                sendLog(log, LogType::Warn, LogFrom::FileIO ,"The pixel format is not support. From" + path);
                return;
            }

            this->data.resize(pixels.size() * sizeof(Vec4<uint8_t>));
            memcpy(this->data.data(), pixels.data(), this->data.size());

            this->size = Vec2<uint32_t>(width, height);
            this->depth = pixel_depth;
        }   
    }
}