#include "ring_buffer.h"

#include <stdio.h>

#define RING_BUFFER_SIZE 4096

static size_t next_pos(const RingBuffer *rb, size_t pos) {
  return (pos + 1) % rb->size;
}

bool ring_buffer_is_empty(const RingBuffer *rb) {
  return rb->write_index == rb->read_index;
}

bool ring_buffer_is_full(const RingBuffer *rb) {
  return next_pos(rb, rb->write_index) == rb->read_index;
}

void ring_buffer_init(RingBuffer *rb, float *storage, size_t size) {
  rb->data = storage;
  rb->size = size;

  rb->write_count = 0;
  rb->read_index = 0;
  rb->write_index = 0;
}

bool ring_buffer_push(RingBuffer *rb, float sample) {
  if (ring_buffer_is_full(rb)) {
    return false;
  }
  rb->data[rb->write_index] = sample;
  rb->write_count++;
  rb->write_index = next_pos(rb, rb->write_index);

  return true;
}

bool ring_buffer_pop(RingBuffer *rb, float *out) {
  if (ring_buffer_is_empty(rb)) {
    return false;
  }
  *out = rb->data[rb->read_index];
  rb->read_index = next_pos(rb, rb->read_index);
  return true;
}
