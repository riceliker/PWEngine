#pragma once
#include <string>
#include <memory>
#include <atomic>
#include <mutex>

#define REMOVE_COPY_FUNCTION_IN_CLASS(T)     \
T(const T&&) = delete;                       \
T& operator=(const T&&) = delete;            \
T(T&&) = delete;                             \
T& operator=(T&&) = delete;                  

#define USE_PIMPL_IN_CLASS  \
struct Impl;                \
std::unique_ptr<Impl> impl; 

namespace PWEngine::Runtime
{
    class Engine;

    enum LogLevel
    {
        Fatal, Error, Warn, Info, Debug, Step
    };

    struct LogInfo
    {
        LogLevel level;
        std::string from;
        std::string info;
    };
    class Log
    {
    public:
        explicit Log(class Engine* engine); 
        REMOVE_COPY_FUNCTION_IN_CLASS(Log)
        USE_PIMPL_IN_CLASS
    private:
        Engine* const engine;
        std::atomic<bool> should_exit{false};
    };

    struct SignalPackage
    {
        std::string str;
        double num;
    };
    /*
     * Event Module [Engine Instance]
     * Event is just record the enum status. Use function call back.
     * The function will be run in the event threads. 
     * So you use make should the threads condition.
     */
    class Event
    {
    public:
        explicit Event(class Engine* engine); 
        REMOVE_COPY_FUNCTION_IN_CLASS(Event)
        USE_PIMPL_IN_CLASS
    private:
        Engine* const engine;
        std::mutex mutex;
        std::condition_variable condition_variable;
        std::atomic<bool> should_exit{false};
    };

    class Engine
    {
    public:
        Engine();
        ~Engine();
        REMOVE_COPY_FUNCTION_IN_CLASS(Engine)
        USE_PIMPL_IN_CLASS

        Log log;
        Event event;
        void Exit();
    private:
        
    };
}

