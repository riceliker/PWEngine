#include "audio.hpp"
#include "_audio.hpp"

#include <AL/alc.h>
#include <AL/al.h>
#include <memory>

namespace PWEngine::Audio 
{
	AudioContext::AudioContext(AudioInstance* instance, AudioContextInfo info) : self(std::make_unique<Impl>())
    {
        this->p_instance = instance;
        const ALCint attrib_list[] = {
            ALC_FREQUENCY, (int)info.frequency,
            ALC_REFRESH, (int)info.fresh,
            ALC_SYNC, info.is_sync?ALC_TRUE:ALC_FALSE,
            0 // EOF
        };
        this->self->context = alcCreateContext(instance->self->device, attrib_list); 
    }

    AudioContext::~AudioContext()
    {
        alcDestroyContext(this->self->context);
    }
}