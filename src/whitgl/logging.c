#include <stdarg.h>
#include <stdio.h>

#include <whitgl/logging.h>

#define LOG_BUFFER_MAX (256)
char _buffer[LOG_BUFFER_MAX];

void whitgl_logit(const char *file, const int line, const char *str, ...)
{
	va_list args;
	va_start(args, str);
	vsnprintf(_buffer, LOG_BUFFER_MAX, str, args);
	printf("%24s:%03d  %s\n", file, line, _buffer);
}

void whitgl_panic(const char *file, const int line, const char *str, ...)
{
	va_list args;
	va_start(args, str);
	vsnprintf(_buffer, LOG_BUFFER_MAX, str, args);
	printf("PANIC %24s:%03d  %s\n", file, line, _buffer);
	printf("PANIC %f", *(double*)0); // do something impossible to crash
}