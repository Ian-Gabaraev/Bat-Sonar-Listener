#pragma once

#define MAX_DEVICES 16
#define MAX_DEVICE_NAME 256

typedef struct {
    char device_name[MAX_DEVICE_NAME];
    int maxInputChannels;
    int maxOutputChannels;
    double defaultSampleRate;
} AudioDevice;

typedef struct {
    int device_count;
    int devices[MAX_DEVICES];
} AvailableDevices;

void get_audio_devices();
AvailableDevices get_ultrasonic_devices();
void get_host_api_info(int index);