#include "audio.hpp"
#include "_audio.hpp"
#include "utils.hpp"

#include <AL/alc.h>
#include <AL/al.h>
#include <memory>

namespace PWEngine::Audio 
{
	AudioInstance::AudioInstance(LogSystem* log) : self(std::make_unique<Impl>())
    {
        this->log = log;
        this->self->device = alcOpenDevice(nullptr);
    }

    AudioInstance::~AudioInstance()
    {
        alcCloseDevice(this->self->device);
    }

    void AudioInstance::currentContext(AudioContext* context)
    {
        alcMakeContextCurrent(context->self->context);
    }
}
