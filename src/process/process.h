#pragma once
#include <stddef.h>
#include <stdint.h>

#include "../buffer/synchronous_single_buffer.h"

typedef struct {
  SynchronousSingleBuffer* buffer;
  int buffer_size;
} ReaderContext;

typedef struct {
  double rms;  // Root-median square
  double zcr;  // Zero-crossing rate
} AudioFeatures;

void* reader_thread(const ReaderContext* rc);
double get_rms(const int16_t* arr, size_t num_of_elements);
double get_zcr(const int16_t* arr, size_t num_of_elements);