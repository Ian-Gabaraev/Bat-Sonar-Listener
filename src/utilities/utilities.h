#pragma once
#include <stddef.h>

typedef struct {
    char hostname[256];
    char cpu_name[256];
    unsigned long ram_mb;
    unsigned long uptime_secs;
} SystemSummary;

int in_array(const unsigned *arr, size_t len, int value);
void suppress_alsa_errors();
void help();
void collect_info(SystemSummary *summary);
