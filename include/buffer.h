#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "audio.h"

typedef struct {
    uint64_t write_count;
    uint32_t writing_at;
    uint32_t reading_at;
    AudioFeatures *storage;
    bool producer_online;
    uint32_t skipped_count;
} FeaturesSyncBuffer;

typedef struct {
    uint64_t write_count;
    uint32_t writing_at;
    uint32_t reading_at;
    int16_t *storage;
    bool producer_online;
    uint32_t skipped_samples_count;
} ProcessingSyncBuffer;

bool init_psb(ProcessingSyncBuffer *buffer, int16_t *storage);
bool psb_blocked(ProcessingSyncBuffer *buffer);
bool psb_empty(const ProcessingSyncBuffer *buffer);
bool psb_full(const ProcessingSyncBuffer *buffer);
bool write_to_psb(ProcessingSyncBuffer *buffer, int16_t value);
bool psb_overwrite(ProcessingSyncBuffer *buffer);
bool rewind_psb(ProcessingSyncBuffer *buffer);

int16_t *read_from_psb(ProcessingSyncBuffer *buffer);

bool init_fsb(FeaturesSyncBuffer *buffer, AudioFeatures *storage);
