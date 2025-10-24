#include "capture.h"

#include <stdio.h>
#include <portaudio.h>

#define ULTRASONIC_DEVICE_THRESHOLD_HZ 48000
#define DEVICE_INFO_SEPARATOR "*****************************\n"

void get_host_api_info(const int index) {
    Pa_Initialize();
    const PaHostApiInfo *host_api = Pa_GetHostApiInfo(index);
    printf("Host API: %s\n", host_api->name );
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
}

void get_ultrasonic_devices() {
    Pa_Initialize();
    const int num_devices = Pa_GetDeviceCount();

    for (int i = 0; i < num_devices; i++) {
        const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);
        if (device_info->defaultSampleRate > ULTRASONIC_DEVICE_THRESHOLD_HZ) {
            const float device_sr_khz = device_info->defaultSampleRate / 1000;
            printf("Device index: %d\n", device_info->hostApi);
            printf("Device name: %s\n", device_info->name);
            printf("Max input channels: %d\n", device_info->maxInputChannels);
            printf("Default sample rate: %.0f kHz \n", device_sr_khz);
            get_host_api_info(device_info->hostApi);
            printf(DEVICE_INFO_SEPARATOR);
        }
    }
}