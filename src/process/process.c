#include "process.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int get_amax(const int16_t *arr, const size_t num_of_elements) {
  int32_t max = 0;
  for (size_t i = 0; i < num_of_elements; ++i) {
    const int v = arr[i];
    const int a = v < 0 ? -v : v;
    if (a > max) {
      max = a;
    }
  }
  return max;
}

double get_rms(const int16_t *arr, const size_t num_of_elements) {
  int32_t sum = 0;

  for (int i = 0; i < num_of_elements; i++) {
    sum += arr[i] * arr[i];
  }

  const double mean_value = (double) sum / (double) num_of_elements;
  const double rms = sqrt(mean_value);

  return rms;
}

double get_zcr(const int16_t *arr, const size_t num_of_elements) {
  int32_t sum = 0;

  for (int i = 1; i < num_of_elements; i++) {
    int sign_current, sign_previous;
    if (arr[i] >= 0) {
      sign_current = 1;
    } else {
      sign_current = -1;
    }
    if (arr[i - 1] >= 0) {
      sign_previous = 1;
    } else {
      sign_previous = -1;
    }
    sum += abs(sign_current - sign_previous);
  }
  return (double) sum / (double) num_of_elements;
}

void *reader_thread(const ReaderContext *rc) {
  while (true) {
    if (rc->buffer->producer_online == false) {
      printf("\u2705 Recording finished, exiting reader thread.\n");
      break;
    }
    if (buffer_full(rc->buffer)) {
      const double rms =
          get_rms(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
      const double zcr =
          get_zcr(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
      const int amax =
          get_amax(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
      rewind_buffer(rc->buffer);
    }
  }
  return 0;
}
