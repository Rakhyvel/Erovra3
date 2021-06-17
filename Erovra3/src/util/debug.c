#pragma once
#include "./debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

/*
	Halts the program, allows me to see stack trace when something goes wrong */
void debug_panic(const char* file, int line, const char* function,
    const char* message, ...) {
    va_list args;

    fprintf(stderr, "ERROR: %s:%d in %s(): ", file, line, function);

    va_start(args, message);
    vprintf(message, args);
    printf("\n");
    va_end(args);
    while (1);
}