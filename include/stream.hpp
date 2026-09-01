#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <filesystem>
#include <unordered_map>


namespace PWEngine::Stream
{
    /* Log System
    If you want to use log system. You should create LogSystem.

    */
    namespace Log 
    {
        enum class LogType
        {
            Info, Warn, Error, Debug
        };
        enum class LogFrom
        {
            Debug, VulkanRender, AssetManager, FileIO
        };
        struct LogInfo
        {
            std::string message;
            LogType type;
            LogFrom from;
        };
        class LogSystem
        {
        private:
            bool is_create_file = false;
            std::queue<LogInfo> queue;
            mutable std::mutex mutex;
            std::condition_variable cv;
            std::atomic<bool> is_stop{false};
            std::thread loop_thread;
        public:
            LogSystem(bool is_create_file);
            ~LogSystem();
            bool getIsStop(){return this->is_stop.load();};
            void send(LogType type, LogFrom from, std::string message);
            friend void LogLoop(LogSystem* ptr);
        };
    }

    namespace Config
    {
        class ConfigFile
        {
        private:
            struct KeyHash {size_t operator()(std::array<char, 64> buf) const noexcept;};
            struct KeyEqual {bool operator()(const std::array<char, 64>& a, const std::array<char, 64>& b) const noexcept;};
            bool read_failed = false;
            std::filesystem::path file_path;
            std::unordered_map<std::array<char, 64>, std::array<char, 256>, KeyHash, KeyEqual> kv;
            std::vector<std::array<char, 64>> ks;
            Log::LogSystem* log;
            ConfigFile(){};
            ConfigFile(std::string config_file_path, Log::LogSystem* log);
            ~ConfigFile();
        public:
            void write();
            std::optional<std::string> get(std::string key);
            void set(std::string key, std::string value);
        };
    }
    
}