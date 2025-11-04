#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "audio.h"

typedef struct {
    uint64_t write_count;
    uint32_t writing_at;
    uint32_t reading_at;
    int16_t *storage;
    bool producer_online;
    uint32_t skipped_count;
} ProcessingBuffer;

typedef struct {
    uint64_t write_count;
    uint32_t writing_at;
    uint32_t reading_at;
    AudioFeatures *storage;
    bool producer_online;
    uint32_t skipped_count;
} FeaturesBuffer;
