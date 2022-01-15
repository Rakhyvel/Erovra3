#pragma once
#include "./debug.h"
#include <SDL.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
	Halts the program, allows me to see stack trace when something goes wrong */
void debug_panic(const char* file, int line, const char* function,
    const char* message, ...)
{
#ifndef _DEBUG
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, NULL);
    exit(0);
#else
    va_list args;

    fprintf(stderr, "ERROR: %s:%d in %s(): ", file, line, function);

    va_start(args, message);
    vprintf(message, args);
    printf("\n");
    va_end(args);
    while (1)
        ;
#endif
}