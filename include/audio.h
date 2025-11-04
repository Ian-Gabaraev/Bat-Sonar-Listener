#pragma once

typedef struct {
    double rms; // Root-median square
    double zcr; // Zero-crossing rate
    int amp_max; // Absolute maximum
    float dominant_freq; // Dominant frequency in Hz
} AudioFeatures;
