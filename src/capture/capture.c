#include "capture.h"

#include <stdio.h>
#include <portaudio.h>
#include <string.h>

#define ULTRASONIC_DEVICE_THRESHOLD_HZ 48000
#define DEVICE_INFO_SEPARATOR "*****************************\n"


void get_host_api_info(const int index) {
    Pa_Initialize();
    const PaHostApiInfo *host_api = Pa_GetHostApiInfo(index);
    printf("Host API: %s\n", host_api->name );
    Pa_Terminate();
}

void get_audio_devices() {
    Pa_Initialize();
    const int num_devices = Pa_GetDeviceCount();

    for (int i = 0; i < num_devices; i++) {
        const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);
        printf("Device %d: %s\n", i, device_info->name);
        printf("Default sample rate: %f\n", device_info->defaultSampleRate);
        printf("Max output channels: %d\n", device_info->maxOutputChannels);
        printf("Max input channels: %d\n", device_info->maxInputChannels);
        printf("Default channel count: %d\n", device_info->maxOutputChannels);
        printf(DEVICE_INFO_SEPARATOR);
    }

    Pa_Terminate();
}

void load_ultrasonic_devices(AvailableDevices *available_devices) {
    Pa_Initialize();
    const int num_devices = Pa_GetDeviceCount();

    available_devices->device_count = 0;

    for (int i = 0; i < num_devices; i++) {
        const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);
        if (device_info->defaultSampleRate > ULTRASONIC_DEVICE_THRESHOLD_HZ) {
            AudioDevice dev;
            strncpy(dev.device_name, device_info->name, sizeof(dev.device_name));
            dev.maxInputChannels = device_info->maxInputChannels;
            dev.maxOutputChannels = device_info->maxOutputChannels;
            dev.defaultSampleRateKhz = device_info->defaultSampleRate / 1000;
            dev.device_id = i;
            available_devices->devices[available_devices->device_count++] = dev;
        }
    }
    Pa_Terminate();
}

void describe_available_ultrasonic_devices(AvailableDevices *available_devices) {
    printf("Found potential ultrasonic devices: %d \n", available_devices->device_count);
    for (int i = 0; i < available_devices->device_count; i++) {
        printf("Device #%d: \n", available_devices->devices[i].device_id);
        printf("Name %s\n", available_devices->devices[i].device_name);
        printf("Sampling rate %.0f kHz\n", available_devices->devices[i].defaultSampleRateKhz);
        printf(DEVICE_INFO_SEPARATOR);
    }
}