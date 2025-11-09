#pragma once

#define MAX_DEVICES 16
#define MAX_DEVICE_NAME 256
#include <portaudio.h>
#include <stdint.h>
#include "buffer.h"

typedef struct {
    char device_name[MAX_DEVICE_NAME];
    unsigned device_index;
    unsigned max_input_channels;
    unsigned max_output_channels;
    uint32_t default_sample_rate_hz;
} AudioDevice;

typedef struct {
    unsigned device_count;
    unsigned device_ids[MAX_DEVICES];
    AudioDevice devices[MAX_DEVICES];
} AvailableDevice;

void get_audio_devices();
void load_ultrasonic_devices(AvailableDevice *available_devices);
void get_host_api_info(int index);
void describe_available_ultrasonic_devices(AvailableDevice *available_devices);
int start_stream(uint32_t frames, AudioDevice *audio_device, ProcessingSyncBuffer *rb, FeaturesSyncBuffer *fb);
void set_up_input_params(PaStreamParameters *streamParameters, const AudioDevice *audio_device);
void display_stream_settings(AudioDevice *audio_device, uint32_t frames);
