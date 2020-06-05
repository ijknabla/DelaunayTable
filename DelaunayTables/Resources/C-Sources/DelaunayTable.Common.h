
#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define MALLOC  (malloc)
#define CALLOC  (calloc)
#define REALLOC (realloc)
#define FREE    (free)


#define SUCCESS ( 0)
#define FAILURE (-1)

enum Verbosity {
    Verbosity__quiet = 1,
    Verbosity__info,
    Verbosity__debug,
    Verbosity__detail
};


#if !defined(NoModelicaStdLib)

#define Runtime__send_message (ModelicaFormatMessage)
#define Runtime__send_error   (ModelicaFormatError)

#else /* NoModelicaStdLib */

#include <stdarg.h>
#include <stdnoreturn.h>

          static inline void Runtime__send_message(const char*, ...);
_Noreturn static inline void Runtime__send_error  (const char*, ...);

/// Alternative to `ModelicaFormatMessage`
static inline void Runtime__send_message(
    const char* const format, ...
) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end  (args);

    // `ModelicaFormatMessage` feeds line on each call
    fprintf(stderr, "\n");
}

/// Alternative to `ModelicaFormatError`
_Noreturn static inline void Runtime__send_error(
    const char* const format, ...
) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end  (args);

    // `ModelicaFormatError` feeds line on each call
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}

#endif  /* !defined(NoModelicaStdLib) */
