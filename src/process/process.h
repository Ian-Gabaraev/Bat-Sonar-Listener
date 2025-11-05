#pragma once
#include <stddef.h>
#include <stdint.h>

#include "../../include/reader.h"

void *reader_thread(const ReaderContext *rc);
double get_rms(const int16_t *arr, size_t num_of_elements);
double get_zcr(const int16_t *arr, size_t num_of_elements);
int get_amp_max(const int16_t *arr, size_t num_of_elements);
float get_dominant_freq(const ReaderContext *rc, size_t num_of_elements);
