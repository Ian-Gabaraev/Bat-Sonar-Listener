#include "utilities.h"

#include <stdio.h>

int in_array(const unsigned *arr, const size_t len, const int value) {
  for (size_t i = 0; i < len; i++) {
    if (arr[i] == value) return 1;
  }
  return 0;
}

void suppress_alsa_errors() {
  if (!freopen("/dev/null", "w", stderr)) {
    perror("freopen failed");
  }
}
