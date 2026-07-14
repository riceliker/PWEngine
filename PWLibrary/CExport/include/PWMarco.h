#include <stdio.h>
#include <string.h>

/// ===== | Both C and C++ support header file | ===== ///
#ifdef __cplusplus
#  define PW_EXTERN_C_BEGIN extern "C" {
#  define PW_EXTERN_C_END   }
#else
#  define PW_EXTERN_C_BEGIN
#  define PW_EXTERN_C_END
#endif

/* Public struct define */
#ifdef __cplusplus
#  define PWL_STRUCT(Name) struct Name;
#else
#  define PWL_STRUCT(Name) typedef struct Name Name; struct Name;
#endif

/* Public enum define */
#ifdef __cplusplus
#  define PWL_ENUM(Name) enum Name
#else
#  define PWL_ENUM(Name) typedef enum Name Name; enum Name
#endif

/// ===== | Null checker | ===== ///

static inline void* PWL_CheckNull(void* self, const char* name)
{
        if(self== NULL)
        {
                printf("\033[33m" "WARN:The pointer(%s) is null." "\033[0m" "\n", name); 
        }
        return self;
}

#define PWL_ReturnNull(self, name, obj) \
do{ \
        PWL_CheckNull((self), name); \
        if (self== NULL) return obj; \
}while(0);

#define PWL_ReturnNullVoid(self, name) \
do{ \
        PWL_CheckNull((self), name); \
        if (self== NULL) return; \
}while(0);