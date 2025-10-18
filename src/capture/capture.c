#include <stdio.h>
#include <portaudio.h>


void get_audio_devices() {
    Pa_Initialize();
    int num_devices = Pa_GetDeviceCount();

    for (int i = 0; i < num_devices; i++) {
        const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);
        printf("Device %d: %s\n", i, device_info->name);
    }
}