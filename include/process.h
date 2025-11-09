#pragma once

#include <stddef.h>
#include <time.h>
#include "buffer.h"

typedef struct {
    ProcessingSyncBuffer *buffer;
    FeaturesSyncBuffer *fsb;
    unsigned buffer_size;
    uint32_t sampling_rate_hz;
} ProcessContext;

void *reader_thread(const ProcessContext *rc);
double get_rms(const int16_t *arr, size_t num_of_elements);
double get_zcr(const int16_t *arr, size_t num_of_elements);
int get_amp_max(const int16_t *arr, size_t num_of_elements);
float get_dominant_freq(const ProcessContext *rc, size_t num_of_elements);
void process(const ProcessContext *rc, struct timespec *start, struct timespec *end);
