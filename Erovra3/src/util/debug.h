#pragma once
#include <stdio.h>
#include <stdarg.h>
#ifndef DEBUG_H
#define DEBUG_H

/* GCC lets us add "attributes" to functions, function
   parameters, etc. to indicate their properties.
   See the GCC manual for details. */
#define UNUSED __attribute__ ((unused))
#define NO_RETURN __attribute__ ((noreturn))
#define NO_INLINE __attribute__ ((noinline))
#define PRINTF_FORMAT(FMT, FIRST) __attribute__ ((format (printf, FMT, FIRST)))

#define PANIC(...) debug_panic (__FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG(...) debug_log (__FILE__, __LINE__, __func__, __VA_ARGS__)

void debug_panic(const char* file, int line, const char* function,
    const char* message, ...);

#define ASSERT(CONDITION)                            \
    if (CONDITION) {                                 \
    } else {                                         \
        PANIC("assertion '%s' failed.", #CONDITION); \
    }

#endif