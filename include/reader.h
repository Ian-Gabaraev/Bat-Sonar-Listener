#pragma once
#include "buffer.h"

typedef struct {
    ProcessingSyncBuffer *buffer;
    unsigned buffer_size;
    uint32_t sampling_rate_hz;
} ReaderContext;
