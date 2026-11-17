#include "gpu.hpp"
#include "utils.hpp"
#include "gpu/instance/_instance.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace PWEngine::GPU 
{
    void GPUInstance::launchTask(uint64_t task_id)
    {
        std::unique_lock<std::timed_mutex> lk(this->task_mtx, std::defer_lock);
        bool got_lock = false;
        while (!got_lock && !is_stop.load()) {
            got_lock = lk.try_lock_for(std::chrono::milliseconds(1));
        }
        if (this->is_stop.load()) return;
        auto result = this->gpu_tasks.find(task_id);
        if ( result == this->gpu_tasks.end())
        {
            sendLog(this->log, LogType::Warn, LogFrom::VulkanRender, "Can not find this task id:"+std::to_string(task_id));
            return;
        }
        WindowTask* task = result->second;
        lk.unlock();

        std::thread task_thread([this, task](){
            double delta = 0;
            while (!this->is_stop.load()) 
            {
                auto start_time = std::chrono::high_resolution_clock::now();
                if (task->task_loop(delta)) break;
                auto current_time = std::chrono::high_resolution_clock::now();
                delta = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
            }
            this->live_task.fetch_sub(1);
            return ;
        });
        task_thread.detach();
        this->live_task.fetch_add(1);
    }

    void GPUInstance::pollEvent()
    {
        glfwPollEvents();
    }
    
    void GPUInstance::waiteIdle()
    {
        vkDeviceWaitIdle(this->self->device);
    }
}