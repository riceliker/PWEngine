#pragma once

#include<format>

using namespace std;
namespace PWL::Log 
{
    inline void LogInfo(const char* fmt)
    {
        printf("\033[34m" "INFO:%s" "\033[0m" "\n", fmt);
    }

    inline void LogWarn(const char *fmt)
    {
        printf("\033[33m" "Warn:%s" "\033[0m" "\n", fmt);     
    }

    inline void LogError(const char *fmt)
    {
        printf("\033[31m" "Error:%s" "\033[0m" "\n", fmt);
    }
}