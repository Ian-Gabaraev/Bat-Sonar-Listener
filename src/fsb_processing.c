#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/buffer.h"
#include "../include/logger.h"

extern uint32_t BUFFER_SIZE;
extern bool LIVE_DEBUG;

bool init_fsb(FeaturesSyncBuffer *buffer, AudioFeatures *storage) {
    if (storage == NULL) {
        if (LIVE_DEBUG) fprintf(stderr, "Buffer storage is NULL\n");
        log_error("Buffer storage is NULL");
        exit(EXIT_FAILURE);
    }
    buffer->write_count = 0;
    buffer->reading_at = 0;
    buffer->writing_at = 0;
    buffer->skipped_count = 0;
    buffer->storage = storage;
    buffer->producer_online = true;

    if (LIVE_DEBUG) fprintf(stdout, "Features buffer initialized with size: %d\n", BUFFER_SIZE);
    log_info("Features buffer initialized with size: %d", BUFFER_SIZE);

    return true;
};
