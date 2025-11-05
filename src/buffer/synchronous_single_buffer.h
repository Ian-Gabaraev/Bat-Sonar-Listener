#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "../../include/buffer.h"

void destroy_buffer(SynchronousSingleBuffer *buffer);

bool init_buffer(SynchronousSingleBuffer *buffer, int16_t *storage);
bool buffer_blocked(SynchronousSingleBuffer *buffer);
bool buffer_empty(const SynchronousSingleBuffer *buffer);
bool buffer_full(const SynchronousSingleBuffer *buffer);
bool write_to_buffer(SynchronousSingleBuffer *buffer, int16_t value);
bool buffer_overwrite(SynchronousSingleBuffer *buffer);
bool rewind_buffer(SynchronousSingleBuffer *buffer);

int16_t *read_from_buffer(SynchronousSingleBuffer *buffer);
