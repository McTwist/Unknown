#include "debug.hpp"

#include <stdarg.h> // va_args
#include <stdio.h> // vfprintf
#include <errno.h> // stderr

#define DEBUG_ENABLED 1

// Printf to screen
void Debug::Log(const char * fmt, ...)
{
#if DEBUG_ENABLED
	va_list list;
	va_start(list, fmt);
	// Use error output as that is logged elsewhere
	vfprintf(stderr, fmt, list);
	va_end(list);
#endif
}
