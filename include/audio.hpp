// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors
#pragma once

#include "format.hpp"
#include "utils.hpp"
#include <cstdint>
#include <memory>

namespace PWEngine::Audio 
{
    struct AudioInstance;
    struct AudioContext;
    struct AudioBuffer;

    struct AudioInstance
    {
        struct Impl;
        std::unique_ptr<Impl> self;
        LogSystem* log;
        AudioInstance(LogSystem* log);
        ~AudioInstance();
        void currentContext(AudioContext* context);
    };

    struct AudioContextInfo
    {
        uint32_t frequency;
        uint32_t fresh;
        bool is_sync;
    };
    
    struct AudioContext
    {
        struct Impl;
        std::unique_ptr<Impl> self;
        AudioInstance* p_instance;
        uint32_t source;
        AudioContext(AudioInstance* instance, AudioContextInfo info);
        ~AudioContext();
    };

    struct AudioBuffer
    {
        uint32_t buffer_id;
        AudioInstance* p_instance;
        AudioBuffer(AudioInstance* instance);
        ~AudioBuffer();
        void load(Format::Audio* audio);
    };

    struct AudioSource
    {
        uint32_t source_id;
        bool is_playing;
        AudioContext* p_context;
        AudioSource(AudioContext* context);
        ~AudioSource();
        void playInLoop(AudioBuffer* buffer, float gain);
    };
}
