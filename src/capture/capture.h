#pragma once

#define MAX_DEVICES 16
#define MAX_DEVICE_NAME 256

#include "../buffer/synchronous_single_buffer.h"

typedef struct {
  char device_name[MAX_DEVICE_NAME];
  int device_index;
  int max_input_channels;
  int max_output_channels;
  double default_sample_rate_hz;
} AudioDevice;

typedef struct {
  int device_count;
  int device_ids[MAX_DEVICES];
  AudioDevice devices[MAX_DEVICES];
} AvailableDevice;

void get_audio_devices();
void load_ultrasonic_devices(AvailableDevice *available_devices);
void get_host_api_info(int index);
void describe_available_ultrasonic_devices(AvailableDevice *available_devices);
int start_stream(int frames, AudioDevice *audio_device,
                 SynchronousSingleBuffer *rb);
