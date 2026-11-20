#include "audio.hpp"
#include "utils.hpp"

#include <AL/alc.h>
#include <AL/al.h>

namespace PWEngine::Audio 
{
    AudioBuffer::AudioBuffer(AudioInstance* instance)
    {
        this->p_instance = instance;
        alGenBuffers(1, &this->buffer_id);
    }

    AudioBuffer::~AudioBuffer()
    {
        alDeleteBuffers(1, &this->buffer_id);
    }

    void AudioBuffer::load(Format::Audio* audio)
    {
        ALenum format;
        if (audio->channels == 1 && audio->bits_per_sample == 8) {
            format = AL_FORMAT_MONO8;
        } else if (audio->channels == 1 && audio->bits_per_sample == 16) {
            format = AL_FORMAT_MONO16;
        } else if (audio->channels == 2 && audio->bits_per_sample == 8) {
            format = AL_FORMAT_STEREO8;
        } else if (audio->channels == 2 && audio->bits_per_sample == 16) {
            format = AL_FORMAT_STEREO16;
        } else {
            sendLog(this->p_instance->log, LogType::Warn, LogFrom::Audio, "Error Audio Format.");
        }

        alBufferData(this->buffer_id, format, audio->data.data(), audio->data.size(), audio->sample_rate);
    }
}