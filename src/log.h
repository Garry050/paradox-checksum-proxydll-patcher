#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void log_init(HINSTANCE hinstDLL);
void log_close(void);
void log_write(const char *level, const char *fmt, ...);

#define LOG_INFO(...)  log_write("INFO",  __VA_ARGS__)
#define LOG_ERROR(...) log_write("ERROR", __VA_ARGS__)
#define LOG_TRACE(...) log_write("TRACE", __VA_ARGS__)
