#pragma once
#include <stddef.h>

#include "../../include/system.h"

int in_array(const unsigned *arr, size_t len, int value);
void suppress_alsa_errors();
void help();
void collect_info(SystemSummary *summary);
