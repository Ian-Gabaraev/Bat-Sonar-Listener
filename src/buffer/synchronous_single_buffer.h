#pragma once
#include <stdbool.h>
#include <stdint.h>

#define SYNCHRONOUS_SINGULAR_BUFFER_SIZE 128000

typedef struct {
  uint64_t write_count;
  uint32_t writing_at;
  uint32_t reading_at;
  int16_t *storage;
  bool producer_online;
  uint32_t skipped_samples_count;
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
