#include <_abort.h>
#include <stdio.h>

/// >>>>>| Console Color |============================================================================================== ///

#define PWE_CONSOLE_RESET          "\033[0m"
#define PWE_CONSOLE_RED            "\033[31m"
#define PWE_CONSOLE_GREEN          "\033[32m"
#define PWE_CONSOLE_YELLOW         "\033[33m"
#define PWE_CONSOLE_BLUE           "\033[34m"

/// >>>>>| Log Output |================================================================================================= ///

void _PWL_LogInfo(const char* fmt, const char* file, int line)
{
        printf(PWE_CONSOLE_BLUE "[%s](%d)INFO:" PWE_CONSOLE_RESET, file, line);
        printf(PWE_CONSOLE_BLUE "%s" PWE_CONSOLE_RESET "\n", fmt);
}

void _PWL_LogWarn(const char *fmt, const char* file, int line)
{
        printf(PWE_CONSOLE_YELLOW "[%s](%d)WARN:" PWE_CONSOLE_RESET, file, line);
        printf(PWE_CONSOLE_YELLOW "%s" PWE_CONSOLE_RESET "\n", fmt);     
}

void _PWL_LogError(const char *fmt, const char* file, int line)
{
        printf(PWE_CONSOLE_RED "[%s](%d)ERROR:" PWE_CONSOLE_RESET, file, line);
        printf(PWE_CONSOLE_RED "%s" PWE_CONSOLE_RESET "\n", fmt);
        abort();
}

#define PWL_LogInfo(fmt) do{ \
_PWL_LogInfo(fmt, __FILE__, __LINE__); }while(0)

#define PWL_LogWarn(fmt) do{ \
_PWL_LogWarn(fmt, __FILE__, __LINE__); }while(0)

#define PWL_LogError(fmt) do{ \
_PWL_LogError(fmt, __FILE__, __LINE__); }while(0)

/// >>>>>| Is NULL Check |=============================================================================================== ///

static inline void* _PWL_CheckNull(void* self, const char* name, const char* file, int line)
{
        if(self == NULL)
        {
                printf(PWE_CONSOLE_YELLOW "[%s](%d)WARN:" PWE_CONSOLE_RESET, file, line);
                printf(PWE_CONSOLE_YELLOW "The pointer(%s) is null." PWE_CONSOLE_RESET "\n", name); 
        }
        return self;
}

#define PWL_CheckNull(self, name) \
_PWL_CheckNull((self), name, __FILE__, __LINE__);

#define PWL_ReturnNull(self, name, obj) \
do{ \
        _PWL_CheckNull((self), name, __FILE__, __LINE__); \
        if (self == NULL) return obj; \
}while(0);

#define PWL_ReturnNullVoid(self, name) \
do{ \
        _PWL_CheckNull((self), name, __FILE__, __LINE__); \
        if (self == NULL) return; \
}while(0);