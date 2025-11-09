#include "../include/logger.h"

#include <stdarg.h>
#include <time.h>

FILE *log_file = NULL;

void logger_init(const char *path) { log_file = fopen(path, "a"); }

void logger_close(void) {
    if (log_file) {
        fprintf(log_file, DELIMITER);
        fprintf(log_file, "\n");
        fclose(log_file);
    }
}

void log_info(const char *fmt, ...) {
    if (!log_file)
        return;
    va_list args;
    va_start(args, fmt);
    fprintf(log_file, "[INFO] %lu ", time(NULL));
    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    va_end(args);
    fflush(log_file);
}

void log_error(const char *fmt, ...) {
    if (!log_file)
        return;
    va_list args;
    va_start(args, fmt);
    fprintf(log_file, "[ERROR] %lu ", time(NULL));
    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    va_end(args);
    fflush(log_file);
}

void log_warning(const char *fmt, ...) {
    if (!log_file)
        return;
    va_list args;
    va_start(args, fmt);
    fprintf(log_file, "[WARN] %lu ", time(NULL));
    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    va_end(args);
    fflush(log_file);
}
