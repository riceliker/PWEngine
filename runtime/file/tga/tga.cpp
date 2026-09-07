#include "file.hpp"
#include "stream.hpp"
#include "../fileio.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace PWEngine::File
{
    TgaStream::TgaStream(Stream::LogSystem* log, std::string load_path)
    {
        auto file_maybe = readBinaryFile(log, load_path);
        size_t offset = 0;
        uint16_t width;
        uint16_t height;
        if (file_maybe.has_value())
        {
            auto file = file_maybe.value();
            this->info.ID_length = file[0];
            this->info.color_map_type = file[1];
            this->info.image_type = file[2];
            this->info.color_map_origin = su16(file, 3);
            this->info.color_map_length = su16(file, 5);
            this->info.color_max_depth = file[7];
            this->info.x_origin = su16(file, 8);
            this->info.y_origin = su16(file, 10);
            width = su16(file, 12);
            this->info.width = width;
            height = su16(file, 14);
            this->info.height = height;
            this->info.pixel_depth = file[16];
            this->info.image_descriptor = file[17];

            offset += 18;
            // have color map? do.
            if (this->info.color_map_type == 1)
            {
                uint8_t entry_size = (this->info.color_max_depth + 7) / 8;
                offset += (size_t)this->info.color_map_length * entry_size;
            }
            if (this->info.pixel_depth == 24)
            {
                for (size_t i = 0; i < width * height; ++i)
                {
                    uint8_t b = file[offset];
                    uint8_t g = file[offset + 1];
                    uint8_t r = file[offset + 2];
                    uint8_t a = 255;
                    this->pixels.emplace_back(Utils::PixelRGBA8{r, g, b, a});
                    offset += 3;
                }
            }
            else if (this->info.pixel_depth == 32)
            {
                for (size_t i = 0; i < width * height; ++i)
                {
                    uint8_t b = file[offset];
                    uint8_t g = file[offset + 1];
                    uint8_t r = file[offset + 2];
                    uint8_t a = file[offset + 3];
                    this->pixels.emplace_back(Utils::PixelRGBA8{r, g, b, a});
                    offset += 4;
                }
            }
            else 
            {
                Stream::log(log, Stream::LogType::Warn, Stream::LogFrom::FileIO ,"The pixel format is not support.");
            }
        }
        
    }

    std::unique_ptr<Utils::Image> TgaStream::makeImage()
    {
        std::unique_ptr<Utils::Image> image;
        image->data = std::move(this->pixels);
        image->size = Utils::Vec2<size_t>(this->info.width, this->info.height);
        image->depth = this->info.pixel_depth;
        return image;
    }
    
}