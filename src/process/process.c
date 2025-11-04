#include "process.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../dsp/kiss_fft/kiss_fftr.h"

float get_dominant_freq(const ReaderContext *rc, const size_t num_of_elements) {
    float array[num_of_elements];

    for (int i = 0; i < num_of_elements; i++) {
        array[i] = (float) rc->buffer->storage[i];
    }

    kiss_fftr_cfg cfg = kiss_fftr_alloc((int) num_of_elements, 0, NULL, NULL);
    if (!cfg) {
        fprintf(stderr, "KissFFT allocation failed\n");
        return -1.0f;
    }

    kiss_fft_cpx *out = malloc((num_of_elements / 2 + 1) * sizeof(kiss_fft_cpx));
    if (!out) {
        free(cfg);
        return -1.0f;
    }

    // Perform FFT
    kiss_fftr(cfg, array, out);

    // Find the frequency bin with the highest magnitude
    float max_magnitude = 0.0f;
    int max_index = 0;

    for (int i = 1; i < num_of_elements / 2; i++) { // skip DC (i=0)
        const float re = out[i].r;
        const float im = out[i].i;
        const float mag = re * re + im * im; // use power for efficiency

        if (mag > max_magnitude) {
            max_magnitude = mag;
            max_index = i;
        }
    }

    // Convert index to frequency in Hz
    const float dominant_freq = (float) max_index * (float) rc->sampling_rate_hz / (float) num_of_elements;

    free(out);
    free(cfg);

    return dominant_freq;
}

int get_amp_max(const int16_t *arr, const size_t num_of_elements) {
    int32_t max = 0;
    for (size_t i = 0; i < num_of_elements; ++i) {
        const int v = arr[i];
        const int a = v < 0 ? -v : v;
        if (a > max) {
            max = a;
        }
    }
    return max;
}

double get_rms(const int16_t *arr, const size_t num_of_elements) {
    int32_t sum = 0;

    for (int i = 0; i < num_of_elements; i++) {
        sum += arr[i] * arr[i];
    }

    const double mean_value = (double) sum / (double) num_of_elements;
    const double rms = sqrt(mean_value);

    return rms;
}

double get_zcr(const int16_t *arr, const size_t num_of_elements) {
    int32_t sum = 0;

    for (int i = 1; i < num_of_elements; i++) {
        int sign_current, sign_previous;
        if (arr[i] >= 0) {
            sign_current = 1;
        } else {
            sign_current = -1;
        }
        if (arr[i - 1] >= 0) {
            sign_previous = 1;
        } else {
            sign_previous = -1;
        }
        sum += abs(sign_current - sign_previous);
    }
    return (double) sum / (double) num_of_elements;
}

void process(const ReaderContext *rc, struct timespec *start, struct timespec *end) {
    clock_gettime(CLOCK_MONOTONIC, start);
    const double rms = get_rms(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
    const double zcr = get_zcr(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
    const int amp_max = get_amp_max(rc->buffer->storage, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
    const float dom_freq = get_dominant_freq(rc, SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
    // AudioFeatures audio_features = {rms, zcr, amp_max, dom_freq};

    clock_gettime(CLOCK_MONOTONIC, end);
    double elapsed = (double) (end->tv_sec - start->tv_sec);
    elapsed += (double) (end->tv_nsec - start->tv_nsec) / 1.0e6;
    printf("\u2705 Reader thread elapsed: %.1lf ms\n", elapsed);
}

void *reader_thread(const ReaderContext *rc) {
    struct timespec start, end;
    while (true) {
        if (rc->buffer->producer_online == false) {
            printf("\u2705 Exiting reader thread.\n");
            break;
        }
        if (buffer_full(rc->buffer)) {
            process(rc, &start, &end);
            rewind_buffer(rc->buffer);
        }
    }
    return 0;
}
