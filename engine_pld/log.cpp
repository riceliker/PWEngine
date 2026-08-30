#include "engine.hpp"

#include <fstream>
#include <print>
#include <string>

namespace PWEngine::Runtime
{
    Log::Log(Engine *const engine) : engine(engine), impl(std::make_unique<Impl>())
    {
        this->impl->log_queue = std::queue<LogInfo>();
    }
    void Log::Impl::SendLog(LogLevel level, std::string from, std::string info)
    {
        std::mutex log_mutex;
        std::condition_variable log_condition_variable;
        std::lock_guard<std::mutex> lock(log_mutex);
        this->log_queue.emplace(level, from, std::move(info));
        log_condition_variable.notify_one();
    }

    void LogModuleLaunch(Log& system, Engine* const engine)
    {
        std::mutex log_mutex;
        std::condition_variable log_condition_variable;
        std::ofstream log_file("PWEngine.log");
        bool use_file = true;
        if(!log_file.is_open())
        {
            std::printf("\033[36m""[Fatal][Init]:Can not open the file: PWEngine.log. The file recorder will be closed.\n""\033[0m");
            use_file = false;
        } 
        while (true)
        {
            std::unique_lock<std::mutex> lock(log_mutex);
            log_condition_variable.wait(lock, [&](){
                return !system.impl->log_queue.empty() || engine->impl->should_exit.load();
            });

            if (engine->impl->should_exit.load())
                break;

            auto log = system.impl->log_queue.front();
            system.impl->log_queue.pop();

            std::string color = "";
            std::string level = "";
            std::string from = "";

            switch (log.level)
            {
            case Fatal:
                level = "Fatal";
                std::printf("\033[36m""[%s][%s]:%s\n""\033[0m" , level.c_str(), from.c_str(), log.info.c_str());
                break;
            case Error:
                level = "Error";
                std::printf("\033[31m""[%s][%s]:%s\n""\033[0m" , level.c_str(), from.c_str(), log.info.c_str());
                break;
            case Warn:
                level = "Warn";
                std::printf("\033[33m""[%s][%s]:%s\n""\033[0m" , level.c_str(), from.c_str(), log.info.c_str());
                break;
            case Info:
                level = "Info";
                std::printf("\033[0m""[%s][%s]:%s\n""\033[0m" , level.c_str(), from.c_str(), log.info.c_str());
                break;
            case Debug:
                level = "Debug";
                std::printf("\033[34m""[%s][%s]:%s\n""\033[0m" , level.c_str(), from.c_str(), log.info.c_str());
                break;
            case Step:
                level = "Step";
                std::printf("\033[32m""[%s][%s]:%s\n""\033[0m" , level.c_str(), from.c_str(), log.info.c_str());
                break;
            }
            if (use_file)
            {
                log_file << "[" << log.level << "]" << "[" << log.from << "]:" << log.info << std::endl; 
            }
        }
    }
}
