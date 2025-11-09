#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/buffer.h"
#include "../include/logger.h"

extern uint32_t BUFFER_SIZE;
extern bool LIVE_DEBUG;

bool init_fsb(FeaturesSyncBuffer *buffer, AudioFeatures *storage) {
    if (storage == NULL) {
        if (LIVE_DEBUG)
            fprintf(stderr, "Buffer storage is NULL\n");
        log_error("Buffer storage is NULL");
        exit(EXIT_FAILURE);
    }
    buffer->write_count = 0;
    buffer->reading_at = 0;
    buffer->writing_at = 0;
    buffer->skipped_count = 0;
    buffer->storage = storage;
    buffer->producer_online = true;

    if (LIVE_DEBUG)
        fprintf(stdout, "Features buffer initialized with size: %d\n", BUFFER_SIZE);
    log_info("Features buffer initialized with size: %d", BUFFER_SIZE);

    return true;
};

bool fsb_full(const FeaturesSyncBuffer *buffer) { return buffer->writing_at == BUFFER_SIZE; };
bool fsb_blocked(const FeaturesSyncBuffer *buffer) {
    return fsb_full(buffer) && buffer->reading_at != buffer->writing_at;
};

bool write_to_fsb(FeaturesSyncBuffer *buffer, const AudioFeatures value) {
    if (fsb_full(buffer) && fsb_blocked(buffer)) {
        buffer->skipped_count++;
        return false;
    }
    if (fsb_full(buffer) && !fsb_blocked(buffer)) {
        rewind_fsb(buffer);
    }
    buffer->storage[buffer->writing_at] = value;
    buffer->write_count++;
    buffer->writing_at++;

    return true;
};

bool rewind_fsb(FeaturesSyncBuffer *buffer) {
    buffer->reading_at = 0;
    buffer->writing_at = 0;

    return true;
}
