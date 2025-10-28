#pragma once
#include <stdbool.h>
#include <stdint.h>

#define SYNCHRONOUS_SINGULAR_BUFFER_SIZE 4096

typedef struct {
  uint16_t write_count;
  uint16_t writing_at;
  uint16_t reading_at;
  int16_t *storage;
  bool producer_online;
} SynchronousSingleBuffer;

bool init_buffer(SynchronousSingleBuffer *buffer, int16_t *storage);
bool buffer_blocked(SynchronousSingleBuffer *buffer);
void destroy_buffer(SynchronousSingleBuffer *buffer);
bool buffer_empty(const SynchronousSingleBuffer *buffer);
bool buffer_full(const SynchronousSingleBuffer *buffer);
bool write_to_buffer(SynchronousSingleBuffer *buffer, int16_t value);
int16_t *read_from_buffer(SynchronousSingleBuffer *buffer);
bool buffer_overwrite(SynchronousSingleBuffer *buffer);
bool rewind_buffer(SynchronousSingleBuffer *buffer);
