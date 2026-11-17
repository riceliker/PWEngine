// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2026 riceliker and all contributors

#include <memory>
#include <string>
namespace PWEngine::Audio 
{
    class AudioInstance;
    class AudioContext;

    class AudioInstance
    {
        struct Impl;
        std::unique_ptr<Impl> self;
        AudioInstance();
        std::unique_ptr<AudioContext> createContext();
    };
    
    class AudioContext
    {
        void play(std::string audio_path);
        
    };
}
