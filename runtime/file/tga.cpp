#include "file.hpp"
#include "stream.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace PWEngine::File
{
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
        
    std::unique_ptr<Utils::ImageRGBA8> tgaReader(Stream::LogSystem* log, std::string load_path)
    {
        auto file_maybe = readBinaryFile(log, load_path);
        size_t offset = 0;
        uint16_t width;
        uint16_t height;
        if (file_maybe.has_value())
        {
            auto file = file_maybe.value();
            FileInfo info;
            std::vector<Utils::PixelRGBA8> pixels;
            info.ID_length = file[0];
            info.color_map_type = file[1];
            info.image_type = file[2];
            info.color_map_origin = su16(file, 3);
            info.color_map_length = su16(file, 5);
            info.color_max_depth = file[7];
            info.x_origin = su16(file, 8);
            info.y_origin = su16(file, 10);
            width = su16(file, 12);
            info.width = width;
            height = su16(file, 14);
            info.height = height;
            info.pixel_depth = file[16];
            info.image_descriptor = file[17];

            offset += 18;
            // have color map? do.
            if (info.color_map_type == 1)
            {
                uint8_t entry_size = (info.color_max_depth + 7) / 8;
                offset += (size_t)info.color_map_length * entry_size;
            }
            if (info.pixel_depth == 24)
            {
                for (size_t i = 0; i < width * height; ++i)
                {
                    uint8_t b = file[offset];
                    uint8_t g = file[offset + 1];
                    uint8_t r = file[offset + 2];
                    uint8_t a = 255;
                    pixels.emplace_back(Utils::PixelRGBA8{r, g, b, a});
                    offset += 3;
                }
            }
            else if (info.pixel_depth == 32)
            {
                for (size_t i = 0; i < width * height; ++i)
                {
                    uint8_t b = file[offset];
                    uint8_t g = file[offset + 1];
                    uint8_t r = file[offset + 2];
                    uint8_t a = file[offset + 3];
                    pixels.emplace_back(Utils::PixelRGBA8{r, g, b, a});
                    offset += 4;
                }
            }
            else 
            {
                Stream::log(log, Stream::LogType::Warn, Stream::LogFrom::FileIO ,"The pixel format is not support.");
            }
            std::unique_ptr<Utils::ImageRGBA8> obj = std::make_unique<Utils::ImageRGBA8>();
            std::vector<uint8_t> data;
            data.resize(pixels.size() * sizeof(Utils::PixelRGBA8));
            memcpy(data.data(), pixels.data(), data.size());
            obj->data = std::move(data);
            obj->size = Utils::Vec2<uint32_t>(info.width, info.height);
            obj->depth = info.pixel_depth;
            return obj;
        } 
        return nullptr;
    }  
}