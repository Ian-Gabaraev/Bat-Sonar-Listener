#pragma once

#include <stdio.h>

#define DELIMITER "**************************************************************"

extern FILE *log_file; // declaration only

void logger_init(const char *path);
void logger_close(void);
void log_info(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_warning(const char *fmt, ...);
