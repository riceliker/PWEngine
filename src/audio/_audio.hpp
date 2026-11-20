#pragma once

#include "AL/alc.h"

#include "audio.hpp"

namespace PWEngine::Audio 
{
    struct AudioInstance::Impl
    {
        ALCdevice* device;
    };

    struct AudioContext::Impl
    {
        ALCcontext* context;
    };
}