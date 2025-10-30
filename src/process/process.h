#pragma once
#include <stddef.h>
#include <stdint.h>

#include "../buffer/synchronous_single_buffer.h"

typedef struct {
  SynchronousSingleBuffer* buffer;
  unsigned buffer_size;
  uint32_t sampling_rate_hz;
} ReaderContext;

typedef struct {
  double rms;           // Root-median square
  double zcr;           // Zero-crossing rate
  int amp_max;          // Absolute maximum
  float dominant_freq;  // Dominant frequency in Hz
} AudioFeatures;

void* reader_thread(const ReaderContext* rc);
double get_rms(const int16_t* arr, size_t num_of_elements);
double get_zcr(const int16_t* arr, size_t num_of_elements);
int get_amp_max(const int16_t* arr, size_t num_of_elements);
float get_dominant_freq(const ReaderContext* rc, size_t num_of_elements);