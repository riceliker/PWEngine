#include "format.hpp"
#include "scene.hpp"
#include "hud.hpp"

#include "audio.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "utils.hpp"
#include "file/wav.hpp"

#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>


struct MyTask : public PWEngine::GPU::WindowTask
{
    std::unique_ptr<ScenePipeline> scene_pipeline;
    std::unique_ptr<HUDPipeline> hud_pipeline;
    std::unique_ptr<PWEngine::Audio::AudioContext> audio_context;
    std::unique_ptr<PWEngine::Audio::AudioBuffer> audio_buffer;
    std::unique_ptr<PWEngine::Format::Audio> music;
    std::unique_ptr<PWEngine::Audio::AudioSource> source;
    bool is_on_play = false;

    MyTask(PWEngine::GPU::GPUInstance* instance, PWEngine::GPU::WindowInfo info, PWEngine::Audio::AudioInstance* audio_instance) : PWEngine::GPU::WindowTask(instance, info)
    {
        this->scene_pipeline = std::make_unique<ScenePipeline>(instance);
        this->hud_pipeline = std::make_unique<HUDPipeline>(instance);
        PWEngine::Audio::AudioContextInfo audio_info = {
            44100, 60, false
        };

        this->audio_context = std::make_unique<PWEngine::Audio::AudioContext>(audio_instance, audio_info);
        this->audio_context->p_instance->currentContext(this->audio_context.get());

        this->audio_buffer = std::make_unique<PWEngine::Audio::AudioBuffer>(audio_instance);
        auto [_music, err_code] = PWEngine::Wav::WavFile("./assets/game_music.wav");
        this->music = std::move(_music);
        PWEngine::sendLog(this->p_instance->log, PWEngine::LogType::Debug, PWEngine::LogFrom::Debug, std::format("Open obj file: Path:{}, Result: {}", "./assets/game_music.wav", static_cast<uint8_t>(err_code)));
        this->audio_buffer->load(music.get());
        this->source = std::make_unique<PWEngine::Audio::AudioSource>(this->audio_context.get());
    }

    bool process(double delta) override
    {  
        source->playInLoop(this->audio_buffer.get(), 0.5);
        
        if (this->m_window->input != nullptr) this->scene_pipeline->camera->updateCamera(this->m_window.get());
        /* mesh */

        std::vector<PWEngine::Mat4> mats = {
            PWEngine::transform({0, 0, 0}, {1, 1, 1}, PWEngine::euler2quat({0, 0, 0})),
            PWEngine::transform({2, 0, 0}, {1, 1, 1}, PWEngine::euler2quat({0, 0, 90})),
        };

        this->scene_pipeline->model_mesh->setData(&mats);

        std::vector<PWEngine::Mat4> ground_mats = {
            PWEngine::transform({0, 0, -0.02}, {10, 10, 0.02}, PWEngine::euler2quat({0, 0, 0})),
        };
        
        this->scene_pipeline->ground->setData(&ground_mats);

        this->recordCommand([&](PWEngine::GPU::WindowRecordCommand* cmd){
            cmd->setScissor({0, 0}, this->m_window->getWindowSize());
            cmd->setViewPort({0, 0}, this->m_window->getWindowSize(), {0, 1});
            auto result = this->getResultImage();

            cmd->bindPipeline(this->scene_pipeline->pipeline.get());
            cmd->Rendering(result.get(), {0.52, 0.62, 0.65, 1}, true, true, [&](PWEngine::GPU::RenderingCommand* rendering){
                cmd->bindBinding(this->scene_pipeline->pipeline.get(), {
                    this->scene_pipeline->camera->binding.get(), 
                    this->scene_pipeline->ground->binding.get()
                });
                rendering->bindVertices({this->scene_pipeline->ground->vertex_buffer.get()}, {0});
                rendering->bindIndex(this->scene_pipeline->ground->index_buffer.get(), PWEngine::GPU::InputUintType::u32);
                rendering->draw(this->scene_pipeline->ground->vertex_count, ground_mats.size(), 0, 0);

                cmd->bindBinding(this->scene_pipeline->pipeline.get(), {
                    this->scene_pipeline->camera->binding.get(),
                    this->scene_pipeline->model_mesh->binding.get()
                });
                rendering->bindVertices({this->scene_pipeline->model_mesh->vertex_buffer.get()}, {0});
                rendering->bindIndex(this->scene_pipeline->model_mesh->index_buffer.get(), PWEngine::GPU::InputUintType::u32);
                rendering->draw(this->scene_pipeline->model_mesh->vertex_count, mats.size(), 0, 0);
            });

            cmd->bindPipeline(this->hud_pipeline->pipeline.get());
            cmd->Rendering(result.get(), {0, 0, 0, 0}, false, false, [&](PWEngine::GPU::RenderingCommand* rendering){
                cmd->bindBinding(this->hud_pipeline->pipeline.get(), {
                    this->hud_pipeline->binding.get()
                });
                rendering->bindVertices({this->hud_pipeline->vertex_buffer.get()}, {0});
                rendering->draw(6, 1, 0, 0);
            });
        });
        return false;
    }
};

int main()
{
    PWEngine::LogSystem log(true);

    PWEngine::GPU::GPUInstanceInfo instance_info = {
        true, "Viking_Room", {1, 0, 0}
    };
    PWEngine::GPU::GPUInstance gpu_instance(&log, instance_info);

    PWEngine::GPU::WindowInfo window_info = {
        {1280, 720}, 
        "Viking Room",
        true
    };

    PWEngine::Audio::AudioInstance audio_instance(&log);
    
    auto main_task_id = gpu_instance.createWindowTask<MyTask>(&gpu_instance, window_info, &audio_instance);

    gpu_instance.run(main_task_id, [](){
        
    });
}