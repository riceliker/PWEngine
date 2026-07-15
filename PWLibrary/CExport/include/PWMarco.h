#pragma once

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