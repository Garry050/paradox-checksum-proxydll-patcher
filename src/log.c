#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static FILE *s_log_file = NULL;

void log_init(HINSTANCE hinstDLL) {
    char dll_path[MAX_PATH];
    char log_path[MAX_PATH];

    if (!GetModuleFileNameA(hinstDLL, dll_path, MAX_PATH)) {
        return;
    }

    /* Replace filename with log filename */
    char *last_sep = strrchr(dll_path, '\\');
    if (last_sep) {
        *(last_sep + 1) = '\0';
    }
    _snprintf_s(log_path, MAX_PATH, _TRUNCATE, "%shoi4_patcher.log", dll_path);

    fopen_s(&s_log_file, log_path, "w");
}

void log_close(void) {
    if (s_log_file) {
        fclose(s_log_file);
        s_log_file = NULL;
    }
}

void log_write(const char *level, const char *fmt, ...) {
    time_t t = time(NULL);
    struct tm tm_buf;
    localtime_s(&tm_buf, &t);

    char prefix[64];
    _snprintf_s(prefix, sizeof(prefix), _TRUNCATE,
                "[%02d:%02d:%02d][%s] ",
                tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec, level);

    va_list args, args2;
    va_start(args, fmt);
    va_copy(args2, args);

    /* Always print to console */
    printf("%s", prefix);
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);

    va_end(args);

    /* Also write to log file if open */
    if (s_log_file) {
        fprintf(s_log_file, "%s", prefix);
        vfprintf(s_log_file, fmt, args2);
        fprintf(s_log_file, "\n");
        fflush(s_log_file);
    }

    va_end(args2);
}
