#include "engine.hpp"

#include <cstddef>
#include <cstdio>
#include <mutex>

namespace PWEngine::Runtime
{
    Engine::Engine() : impl(std::make_unique<Impl>()),
    log(this), event(this)
    {
        // Copy right and license.
        // std::printf("PWEngine 0.0.1 copy right by Copyright by riceliker and all contributors. Open source license: LGPL3.0\n");
        // std::printf("SDL Version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
        // std::printf("Runing platform: %s\n", SDL_GetPlatform());

        // Init Log System
        this->impl->AddThread("log",std::move([this](){
            LogModuleLaunch(this->log, this);
        }));

        // Init Signal System
        
    }
    
    void Engine::Exit()
    {
        this->impl->should_exit = true;
        for (const auto& thread: this->impl->deamon_thread)
        {
            std::lock_guard<std::mutex> lock(this->impl->thread_mutex);
            std::move(thread.second)->Exit();
        }
        this->impl->deamon_thread.clear();
    }

    Engine::~Engine()
    {
        Exit();
    }

    template<typename Func>
    void Engine::Impl::AddThread(std::string name, Func&& func)
    {
        std::lock_guard lock(this->thread_mutex);
        auto handle = std::make_unique<Thread>();

        handle->thread = std::thread(std::forward<Func>(func));
        this->deamon_thread.emplace(name, std::move(handle));
    }

    void Engine::Impl::KillTread(std::string name)
    {
        std::lock_guard<std::mutex> lock(this->thread_mutex);
        this->deamon_thread.erase(name);
    }
}