#include "process.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double get_rms(const int arr[], const int num_of_elements) {
  double sum = 0;

  for (int i = 0; i < num_of_elements; i++) {
    sum += pow(arr[i], 2);
  }

  const double mean_value = sum / num_of_elements;
  const double rms = sqrt(mean_value);

  return rms;
}

double get_zcr(const int arr[], const int num_of_elements) {
  double sum = 0;

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
  return sum / num_of_elements;
}

void* reader_thread(const ReaderContext* rc) {
  while (true) {
    if (buffer_full(rc->buffer)) {
      const double rms =
          get_rms(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
      const double zcr =
          get_zcr(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
      rewind_buffer(rc->buffer);
      printf("RMS: %lf\n", rms);
      printf("ZCR: %lf\n", zcr);
    }
  }
}
