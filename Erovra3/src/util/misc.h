#ifndef MISC_H
#define MISC_H

#define EXTRACT_VARARGS(buffer, start_args)      \
    {                                            \
        va_list lst;                             \
        va_start(lst, start_args);               \
        vsnprintf(buffer, 255, start_args, lst); \
        va_end(lst);                             \
    }

#endif