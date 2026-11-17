#include "file/wav.hpp"
#include "al/al.h"
#include "al/alc.h"

int main()
{
    ALCdevice *device = alcOpenDevice(nullptr);

    const ALCint attrib_list[] = {
    ALC_FREQUENCY, 44100,
    ALC_REFRESH, 60,
    ALC_SYNC, ALC_FALSE,
    0 // EOF
    };
    ALCcontext *context = alcCreateContext(device, attrib_list);
    alcMakeContextCurrent(context);

    ALuint buffer;
    alGenBuffers(1, &buffer);
    
    auto [result, err_code] = PWEngine::Wav::WavFile("./assets/game_music.wav");
    
    ALenum format;
    if (result->channels == 1 && result->bits_per_sample == 8) {
        format = AL_FORMAT_MONO8;
    } else if (result->channels == 1 && result->bits_per_sample == 16) {
        format = AL_FORMAT_MONO16;
    } else if (result->channels == 2 && result->bits_per_sample == 8) {
        format = AL_FORMAT_STEREO8;
    } else if (result->channels == 2 && result->bits_per_sample == 16) {
        format = AL_FORMAT_STEREO16;
    } else {
    // 未知格式
    }

    alBufferData(buffer, format, result->data.data(), result->data.size(), result->sample_rate);

    ALuint source;
    alGenSources(1, &source);
    // 设置属性
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_GAIN, 0.5f);

    alSourcePlay(source);
    ALint state = AL_PLAYING;
    while (state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    }

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);

    alcDestroyContext(context);
    alcCloseDevice(device);
}