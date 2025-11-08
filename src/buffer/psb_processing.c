#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../include/buffer.h"

extern uint32_t BUFFER_SIZE;
extern bool DEBUG;
extern uint16_t RECORDING_DURATION_SECONDS;

bool init_psb(ProcessingSyncBuffer *buffer, int16_t *storage) {
    if (storage == NULL) {
        fprintf(stderr, "Buffer storage is NULL\n");
        exit(EXIT_FAILURE);
    }
    buffer->write_count = 0;
    buffer->reading_at = 0;
    buffer->writing_at = 0;
    buffer->skipped_samples_count = 0;
    buffer->storage = storage;
    buffer->producer_online = true;

    if (DEBUG) {
        const time_t now = time(NULL);
        fprintf(stdout, "\033[0;32m[INFO @ %ld] Processing buffer initialized with size: %d\n", now, BUFFER_SIZE);
        fprintf(stdout, "[INFO @ %ld] Mode: AUTO. Listening for %d seconds\n", now, RECORDING_DURATION_SECONDS);
    }

    return true;
};

bool psb_empty(const ProcessingSyncBuffer *buffer) { return buffer->write_count == 0; }

bool rewind_psb(ProcessingSyncBuffer *buffer) {
    buffer->reading_at = 0;
    buffer->writing_at = 0;

    return true;
}

bool psb_full(const ProcessingSyncBuffer *buffer) { return buffer->writing_at == BUFFER_SIZE; };

bool psb_blocked(ProcessingSyncBuffer *buffer) { return psb_full(buffer) && buffer->reading_at != buffer->writing_at; };

bool psb_overwrite(ProcessingSyncBuffer *buffer) {
    return buffer->write_count > 0 && !(buffer->write_count % BUFFER_SIZE);
};

bool write_to_psb(ProcessingSyncBuffer *buffer, const int16_t value) {
    if (psb_full(buffer) && psb_blocked(buffer)) {
        buffer->skipped_samples_count++;
        return false;
    }
    if (psb_full(buffer) && !psb_blocked(buffer)) {
        rewind_psb(buffer);
    }
    buffer->storage[buffer->writing_at] = value;
    buffer->write_count++;
    buffer->writing_at++;

    return true;
};

int16_t *read_from_psb(ProcessingSyncBuffer *buffer) {
    int16_t *value = &buffer->storage[buffer->reading_at];
    buffer->reading_at++;

    return value;
};
