#pragma once

typedef struct {
    char hostname[256];
    char cpu_name[256];
    unsigned long ram_mb;
    unsigned long uptime_secs;
} SystemSummary;
