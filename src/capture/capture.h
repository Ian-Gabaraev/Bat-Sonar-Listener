#pragma once
void get_audio_devices();
void get_ultrasonic_devices();
void get_host_api_info(int index);

typedef struct {
    char device_name[256];
    int maxInputChannels;
    int maxOutputChannels;
    double defaultSampleRate;
} AudioDevice;