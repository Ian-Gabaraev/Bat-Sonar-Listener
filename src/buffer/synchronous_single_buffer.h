#pragma once
#include <stdbool.h>

#define SYNCHRONOUS_SINGULAR_BUFFER_SIZE 4096

typedef struct {
  int write_count;
  int writing_at;
  int reading_at;
  int *storage;
} SynchronousSingleBuffer;

bool init_buffer(SynchronousSingleBuffer *buffer, int *storage);
bool buffer_blocked(SynchronousSingleBuffer *buffer);
void destroy_buffer(SynchronousSingleBuffer *buffer);
bool buffer_empty(const SynchronousSingleBuffer *buffer);
bool buffer_full(const SynchronousSingleBuffer *buffer);
bool write_to_buffer(SynchronousSingleBuffer *buffer, int value);
int *read_from_buffer(SynchronousSingleBuffer *buffer);
bool buffer_overwrite(SynchronousSingleBuffer *buffer);
bool rewind_buffer(SynchronousSingleBuffer *buffer);
