#include "synchronous_single_buffer.h"

#include <stdio.h>
#include <stdlib.h>

bool init_buffer(SynchronousSingleBuffer *buffer, int16_t *storage) {
    if (storage == NULL) {
        fprintf(stderr, "Buffer storage is NULL\n");
        return false;
    }
    buffer->write_count = 0;
    buffer->reading_at = 0;
    buffer->writing_at = 0;
    buffer->skipped_samples_count = 0;
    buffer->storage = storage;
    buffer->producer_online = true;

    return true;
};

bool buffer_empty(const SynchronousSingleBuffer *buffer) { return buffer->write_count == 0; }

bool rewind_buffer(SynchronousSingleBuffer *buffer) {
    buffer->reading_at = 0;
    buffer->writing_at = 0;

    return true;
}

bool buffer_full(const SynchronousSingleBuffer *buffer) {
    return buffer->writing_at == SYNCHRONOUS_SINGULAR_BUFFER_SIZE;
};

bool buffer_blocked(SynchronousSingleBuffer *buffer) {
    return buffer_full(buffer) && buffer->reading_at != buffer->writing_at;
};

bool buffer_overwrite(SynchronousSingleBuffer *buffer) {
    return buffer->write_count > 0 && !(buffer->write_count % SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
};

bool write_to_buffer(SynchronousSingleBuffer *buffer, const int16_t value) {
    if (buffer_full(buffer) && buffer_blocked(buffer)) {
        buffer->skipped_samples_count++;
        return false;
    }
    if (buffer_full(buffer) && !buffer_blocked(buffer)) {
        rewind_buffer(buffer);
    }
    buffer->storage[buffer->writing_at] = value;
    buffer->write_count++;
    buffer->writing_at++;

    return true;
};

int16_t *read_from_buffer(SynchronousSingleBuffer *buffer) {
    int16_t *value = &buffer->storage[buffer->reading_at];
    buffer->reading_at++;

    return value;
};
