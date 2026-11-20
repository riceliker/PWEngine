#include "audio.hpp"
#include "_audio.hpp"
#include "utils.hpp"

#include <AL/alc.h>
#include <AL/al.h>
#include <memory>

namespace PWEngine::Audio 
{
    AudioSource::AudioSource(AudioContext* context)
    {
        this->p_context = context;
    }

    AudioSource::~AudioSource()
    {

    }
    
    void AudioSource::playInLoop(AudioBuffer* buffer, float gain)
    {
        if (!is_playing)
        {
            alGenSources(1, &this->source_id);
            
            alSourcei(this->source_id, AL_BUFFER, buffer->buffer_id);
            alSourcef(this->source_id, AL_GAIN, gain);

            alSourcePlay(this->source_id);
        }

        ALint state = AL_PLAYING;
        alGetSourcei(this->source_id, AL_SOURCE_STATE, &state);

        if (state == AL_PLAYING) is_playing = true;
        else {
            is_playing = false;
            alDeleteSources(1, &this->source_id);
        }
    }
}