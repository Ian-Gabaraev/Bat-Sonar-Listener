#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/buffer.h"

extern uint32_t BUFFER_SIZE;
extern bool DEBUG;

bool init_fsb(FeaturesSyncBuffer *buffer, AudioFeatures *storage) {
    if (storage == NULL) {
        fprintf(stderr, "Buffer storage is NULL\n");
        exit(EXIT_FAILURE);
    }
    buffer->write_count = 0;
    buffer->reading_at = 0;
    buffer->writing_at = 0;
    buffer->skipped_count = 0;
    buffer->storage = storage;
    buffer->producer_online = true;

    if (DEBUG) {
        const time_t now = time(NULL);
        fprintf(stdout, "[INFO @ %ld] Features buffer initialized with size: %d\n", now, BUFFER_SIZE);
    }

    return true;
};
