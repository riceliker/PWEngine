#pragma once
#include "export.hpp"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <thread>
#include <map>
#include <mutex>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>

namespace PWEngine::Runtime
{
    class Thread
    {
    public:
        std::thread thread;
        std::condition_variable cv;
        std::mutex mutex;
        REMOVE_COPY_FUNCTION_IN_CLASS(Thread)

        Thread(){}

        void Exit()
        {
            cv.notify_one();
            if (thread.joinable())
                thread.join();
        }
    };

    struct Engine::Impl
    {
        std::atomic<bool> should_exit{false};
        std::map<std::string, std::unique_ptr<Thread>> deamon_thread;
        std::mutex thread_mutex;
        template<typename Func>
        void AddThread(std::string name, Func&& func);
        void KillTread(std::string name);
    };

    struct Log::Impl
    {
        std::queue<LogInfo> log_queue;
        void SendLog(LogLevel level, std::string from, std::string info);
    };
    void LogModuleLaunch(Log& system, Engine *const engine);
    
    struct Event::Impl
    {
        std::vector<int16_t> event_value;
        std::unordered_map<std::string, size_t> event_name_index;
        void Registry();
        void Set();
        void Read();
    };

    

}