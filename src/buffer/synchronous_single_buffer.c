#include "synchronous_single_buffer.h"

#include <stdio.h>
#include <stdlib.h>

bool init_buffer(SynchronousSingleBuffer *buffer, int *storage) {
  if (storage == NULL) {
    fprintf(stderr, "Buffer storage is NULL\n");
    return false;
  }
  buffer->write_count = 0;
  buffer->reading_at = 0;
  buffer->writing_at = 0;
  buffer->storage = storage;

  return true;
};

bool buffer_empty(const SynchronousSingleBuffer *buffer) {
  return buffer->write_count == 0;
}

bool rewind_buffer(SynchronousSingleBuffer *buffer) {
  buffer->reading_at = 0;
  buffer->writing_at = 0;

  return true;
}

bool buffer_full(const SynchronousSingleBuffer *buffer) {
  return buffer->writing_at == SYNCHRONOUS_SINGULAR_BUFFER_SIZE - 1;
};

bool buffer_blocked(SynchronousSingleBuffer *buffer) {
  return buffer_full(buffer) && buffer->reading_at != buffer->writing_at;
};

bool buffer_overwrite(SynchronousSingleBuffer *buffer) {
  return !(buffer->write_count % SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
};

bool write_to_buffer(SynchronousSingleBuffer *buffer, const int value) {
  if (buffer_full(buffer) && buffer_blocked(buffer)) {
    return false;
  }
  if (buffer_full(buffer) && !buffer_blocked(buffer)) {
    rewind_buffer(buffer);
  }
  if (buffer_overwrite(buffer)) {
    printf("Overwrite\n");
  }
  buffer->storage[buffer->writing_at] = value;
  buffer->write_count++;
  buffer->writing_at++;

  return true;
};

int *read_from_buffer(SynchronousSingleBuffer *buffer) {
  int *value = &buffer->storage[buffer->reading_at];
  buffer->reading_at++;

  return value;
};

void destroy_buffer(SynchronousSingleBuffer *buffer) { free(buffer->storage); };
