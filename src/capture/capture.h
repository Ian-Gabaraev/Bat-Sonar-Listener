#pragma once

#define MAX_DEVICES 16
#define MAX_DEVICE_NAME 256

typedef struct {
    char device_name[MAX_DEVICE_NAME];
    int device_id;
    int maxInputChannels;
    int maxOutputChannels;
    double defaultSampleRateKhz;
} AudioDevice;

typedef struct {
    int device_count;
    AudioDevice devices[MAX_DEVICES];
} AvailableDevices;

void get_audio_devices();
void load_ultrasonic_devices(AvailableDevices *available_devices);
void get_host_api_info(int index);
void describe_available_ultrasonic_devices(AvailableDevices *available_devices);