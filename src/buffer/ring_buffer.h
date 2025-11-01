#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    float *data;
    size_t size;
    size_t read_index;
    size_t write_index;
    size_t write_count;
} RingBuffer;

void ring_buffer_init(RingBuffer *rb, float *storage, size_t size);
bool ring_buffer_push(RingBuffer *rb, float sample);
bool ring_buffer_pop(RingBuffer *rb, float *out);
bool ring_buffer_is_empty(const RingBuffer *rb);
bool ring_buffer_is_full(const RingBuffer *rb);
