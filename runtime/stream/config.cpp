#include "stream.hpp"
#include <cstdio>
#include <format>
#include <fstream>

namespace PWEngine::Stream::Config
{
    /* override hash function for array */
    size_t ConfigFile::KeyHash::operator()(std::array<char, 64> buf) const noexcept
    {
        const char* p = buf.data();
        size_t len = std::strlen(p);
        std::string_view sv(p, len);

        uint64_t h = 14695981039346656037ULL;
        for (char ch : sv)
        {
            h ^= static_cast<uint8_t>(ch);
            h *= 1099511628211ULL;
        }
        return static_cast<size_t>(h);
    }
    /* override compare function for array */
    bool ConfigFile::KeyEqual::operator()(const std::array<char, 64>& a, const std::array<char, 64>& b) const noexcept
    {
        return std::strcmp(a.data(), b.data()) == 0;
    }
    ConfigFile::ConfigFile(std::string config_file_path, Log::LogSystem* log): log(log)
    {
        /* path check */
        this->file_path = config_file_path;
        if (!std::filesystem::exists(config_file_path))
        {
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "Can not find config.txt file.");
            this->read_failed = true;
        }
        if (std::filesystem::is_directory(config_file_path))
        {
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "The config.txt is dictionary. Not a file.");
            this->read_failed = true;
        }
        /* open file */
        std::ifstream file(config_file_path);
        if (!file.is_open())
        {
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "Can not open config.txt file.");
            this->read_failed = true;
        }
        /* read line */
        std::string get_line;
        while (std::getline(file, get_line))
        {
            /* get key & value */
            if (!get_line.empty() && get_line[0] == '#')
                continue;
            bool is_value_string = false;
            std::string key = "";
            std::string value = "";
            for (char word : get_line)
            {
                if (word == ':')
                {
                    is_value_string = true;
                    continue;
                }
                if (is_value_string)
                    value += word;
                else 
                    key += word;
            }
            /* check key & value */
            if (key.length() > 63)
            {
                log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "The key text length can not longer than 64 in config.txt file.");
                return;
            }
            if (value.length() > 255)
            {
                log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "The value text length can not longer than 256 in config.txt file.");
                return;
            }
            /* store it */
            std::array<char, 64> key_array{};
            std::memcpy(key_array.data(), key.data(), key.size());
            key_array[key.size()] = '\0';
            std::array<char, 256> value_array{};
            std::memcpy(value_array.data(), value.data(), value.size());
            value_array[value.size()] = '\0';

            this->kv.insert({key_array, value_array});
        }
        file.close();
    }
    ConfigFile::~ConfigFile()
    {
        this->write();
        std::string normal_name = this->file_path;
        std::string temp_name = std::string("~") + this->file_path.string();
        int ret = std::rename(temp_name.c_str(), normal_name.c_str());
    }
    std::optional<std::string> ConfigFile::get(std::string key)
    {
        if (key.length() > 63)
        {
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "The key text length can not longer than 64 in config.txt file.");
            return std::nullopt;
        }
        std::array<char, 64> key_array{};
        std::memcpy(key_array.data(), key.data(), key.size());
        key_array[key.size()] = '\0';
        auto out = this->kv.find(key_array);
        if (out == this->kv.end())
        {
            std::string msg = std::format("Can no find key (%s) in config.txt file.", key);
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, msg);
            return std::nullopt;
        }
        else 
        {
            ks.push_back(key_array);
            return std::string(out->second.data(), out->second.size());
        }
        
    }
    void ConfigFile::set(std::string key, std::string value)
    {
        /* check key & value */
        if (key.length() > 63)
        {
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "The key text length can not longer than 64 in config.txt file.");
            return;
        }
        if (value.length() > 255)
        {
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "The value text length can not longer than 256 in config.txt file.");
            return;
        }
        std::array<char, 64> key_array{};
        std::memcpy(key_array.data(), key.data(), key.size());
        key_array[key.size()] = '\0';
        std::array<char, 256> value_array{};
        std::memcpy(value_array.data(), value.data(), value.size());
        value_array[value.size()] = '\0';
        /* find key*/
        auto out = this->kv.find(key_array);
        if (out == this->kv.end())
        {
            std::string msg = std::format("Can no find key ({}) in config.txt file.", key);
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, msg);
            return;
        }
        else 
        {
            out->second = value_array;
        }
    }
    void ConfigFile::write()
    {
        /* create temp_file */
        auto temp_file_path = this->file_path;
        temp_file_path.replace_filename(std::string("~") + this->file_path.filename().string());
        std::ofstream file(temp_file_path);
        if (!file.is_open())
        {
            log->send(Log::LogType::Warn, Log::LogFrom::FileIO, "Can not open config.txt file.");
            this->read_failed = true;
        }
        /* write data*/
        for (const auto& key: this->ks)
        {   
            auto out = this->kv.find(key);
            file << out->first.data() << ":" << out->second.data() << std::endl;
        }
        file.close();
    }
}