#include "stream.hpp"
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

namespace PWEngine::Stream
{
    void LogLoop(LogSystem* ptr)
    {
        std::ofstream log_file("PWEngine.log");
        while (!ptr->is_stop.load())
        {
            std::unique_lock<std::mutex> lock(ptr->mutex);
            ptr->cv.wait_for(
                lock,
                std::chrono::milliseconds(16),
                [ptr]() { return ptr->is_stop.load() || !ptr->queue.empty(); });
            if (ptr->is_stop.load())
                break;
            if (ptr->queue.empty())
                continue;
            LogInfo message = ptr->queue.front();
            ptr->queue.pop();

            std::string color;
            std::string type;
            std::string from;
            switch (message.type)
            {
            case LogType::Info:
                color = "";
                type = "Info";
                break;
            case LogType::Warn:
                color = "\033[33m";
                type = "Warn";
                break;
            case LogType::Error:
                color = "\033[31m";
                type = "Error";
                break;
            case LogType::Debug:
                color = "\033[32m";
                type = "Debug";
                break;
            }
            switch (message.from)
            {
            case LogFrom::AssetManager:
                from = "AssetManager";
                break;
            case LogFrom::Debug:
                from = "Debug";
                break;
            case LogFrom::FileIO:
                from = "FileIO";
                break;
            case LogFrom::VulkanRender:
                from = "VulkanRender";
                break;
            }
            std::time_t now_time = std::time(nullptr);
            auto time = std::localtime(&now_time);
            std::string text = "[" + std::to_string(time->tm_hour) + ":" +
                               std::to_string(time->tm_min) + ":" +
                               std::to_string(time->tm_sec) + "][" + from +
                               "]" + message.message;
            std::string color_text = color + text + "\033[0m";
            printf("%s\n", color_text.c_str());
            if (ptr->is_create_file)
            {
                log_file << "<" << type << ">" << text << std::endl;
            }
            if (message.type == LogType::Error)
            {
                std::runtime_error("message");
            }
        }
        log_file.close();
    }

    LogSystem::LogSystem(bool is_create_file) : is_create_file(is_create_file)
    {
        std::thread loop_thread(LogLoop, this);
        this->loop_thread = std::move(loop_thread);
    }

    LogSystem::~LogSystem()
    {
        this->is_stop = true;
        this->cv.notify_all();
        if (this->loop_thread.joinable())
        {
            this->loop_thread.join();
        }
    }

    void LogSystem::send(LogType type, LogFrom from, std::string message)
    {
        LogInfo info{};
        info.type = type;
        info.from = from;
        info.message = message;
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->queue.push(info);
        }
        cv.notify_one();
    }
} // namespace PWEngine::Stream