// SPDX-License-Identifier: MIT
// Copyright (C) 2026 riceliker
/*
MIT License 

Copyright (c) 2026 riceliker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "bitstream.hpp"
#include "format.hpp"
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <vector>
namespace PWEngine::Wav 
{
    enum class WavErrorCode
    {
        Okay = 0,
        CanNotOpenFile = 1,
        NotWavFile = 2,
        ErrorFormat = 3,
        DataLost = 4,
    };

    inline std::tuple<std::unique_ptr<Format::Audio>, WavErrorCode> WavFile(std::string path)
    {
        auto result = std::make_unique<Format::Audio>();

        WavErrorCode err_code = WavErrorCode::Okay;
        auto _file = BitStream::readBinaryFile(path);
        if (!_file.has_value()) 
            return std::make_tuple(std::move(result), WavErrorCode::CanNotOpenFile);
        
        auto file = _file.value();
        if (file.size() < 52) 
            return std::make_tuple(std::move(result), WavErrorCode::DataLost);

        auto ptr = file.data();
        /* RIFF header */
        uint32_t riff_header_number = BitStream::get32le(ptr);
        if (!BitStream::isBlock({'R', 'I', 'F', 'F'}, riff_header_number)) 
            return std::make_tuple(std::move(result), WavErrorCode::NotWavFile);

        uint32_t chunk_size = BitStream::get32le(ptr);
        if (chunk_size < 4) 
            return std::make_tuple(std::move(result), WavErrorCode::NotWavFile);

        uint32_t wav_number = BitStream::get32le(ptr);
        if (!BitStream::isBlock({'W', 'A', 'V', 'E'}, wav_number)) 

            return std::make_tuple(std::move(result), WavErrorCode::NotWavFile);
        /* format chuck */
        uint32_t fmt_number = BitStream::get32le(ptr);
        if (!BitStream::isBlock({'f', 'm', 't', ' '}, fmt_number)) 
            return std::make_tuple(std::move(result), WavErrorCode::ErrorFormat);

        uint32_t fmt_size = BitStream::get32le(ptr);
        if (fmt_size != 16 && fmt_size != 18 && fmt_size != 40) 
            return std::make_tuple(std::move(result), WavErrorCode::ErrorFormat);

        uint16_t format_tag = BitStream::get16le(ptr);
        uint16_t channels = BitStream::get16le(ptr);
        uint32_t samples_per_second = BitStream::get32le(ptr);
        uint32_t average_bytes_per_second = BitStream::get32le(ptr);
        uint16_t block_align = BitStream::get16le(ptr);
        uint16_t bits_per_sample = BitStream::get16le(ptr);

        result->channels = channels;
        result->sample_rate = samples_per_second;
        result->bits_per_sample = bits_per_sample;

        uint16_t cb_size = 0;
        if (fmt_size > 16) {
            cb_size = BitStream::get16le(ptr);
        }
        uint16_t valid_bits_per_sample = 0;
        uint32_t channel_mask = 0;
        if (fmt_size > 18) {
            valid_bits_per_sample = BitStream::get16le(ptr);
            channel_mask = BitStream::get32le(ptr);


            //TODO : sub format
        }

        switch (format_tag) {
        case 0x0001:
            /* PCM */
        break;
        case 0x0003:
        break;
        case 0x0006:
        break;
        case 0x0007:
        break;
        case 0xFFFE:
        break;
        default:
            return std::make_tuple(std::move(result), WavErrorCode::ErrorFormat);
        }

        /* if fact */
        uint32_t body_number = BitStream::get32le(ptr);
        uint32_t sample_length = 0;
        if (BitStream::isBlock({'f', 'a', 'c', 't'}, body_number)) {
            uint32_t fact_size = BitStream::get32le(ptr);
            //if (fact_size != 4) 
            //    return std::make_tuple(std::move(result), WavErrorCode::ErrorFormat);

            uint32_t sample_length = BitStream::get32le(ptr);

            body_number = BitStream::get32le(ptr);
        }
        if (!BitStream::isBlock({'d', 'a', 't', 'a'}, body_number)) 
            return std::make_tuple(std::move(result), WavErrorCode::ErrorFormat);

        uint32_t data_size = BitStream::get32le(ptr);

        result->total_samples = data_size / (channels + average_bytes_per_second);

        std::vector<uint8_t> pcm_data;
        pcm_data.resize(data_size);
        memcpy(pcm_data.data(), ptr, pcm_data.size());

        result->data = std::move(pcm_data);

        return std::make_tuple(std::move(result), WavErrorCode::Okay);
    }
}