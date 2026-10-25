#include "gpu.hpp"
#include "stream.hpp"

class MyTask : public PWEngine::GPU::GPUTaskWithWindow
{
public:
    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo info) : PWEngine::GPU::GPUTaskWithWindow(instance, info)
    {

    }
    bool loop(double delta) override
    {
        this->beginCommandInProcess();
        this->endCommandInProcess();
        return false;
    }
};

int main()
{
    PWEngine::LogSystem log(true);

    PWEngine::GPU::GPUInstanceInfo instance_info = {
        false, "Viking Room", {1, 0, 0}
    };
    PWEngine::GPU::GPUInstance instance(&log, instance_info);

    PWEngine::GPU::WindowInfo window_info = {
        {1280, 720}, 
        "Viking Room",
        true
    };

    auto main_task_id = instance.createGPUTask<MyTask>(&instance, window_info);

    instance.run(main_task_id);


}