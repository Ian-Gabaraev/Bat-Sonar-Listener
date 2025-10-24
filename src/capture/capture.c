#include "capture.h"

#include <portaudio.h>
#include <stdio.h>
#include <string.h>

#define ULTRASONIC_DEVICE_THRESHOLD_HZ 48000
#define DEVICE_INFO_SEPARATOR "*****************************\n"

static int audioCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData) {
    (void) outputBuffer;
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if (inputBuffer) {
        const float *in = inputBuffer;
        printf("%f\n", *in);
    }
    return paContinue;
}

int start_stream(const int frames, const AudioDevice *audio_device) {
    printf("Starting capture from: %s\n", audio_device->device_name);
    printf("Sample size: %d\n", frames);
    printf("Input channels: %d\n", audio_device->maxInputChannels);
    printf("Press Enter to start\n");
    getchar();
    PaError err;
    PaStream *stream = NULL;

    if ((err = Pa_Initialize()) != paNoError) {
        printf("PortAudio init error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    PaStreamParameters inputParams;

    inputParams.device = audio_device->device_id;
    inputParams.channelCount = audio_device->maxInputChannels;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(audio_device->device_id)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, &inputParams, NULL, audio_device->defaultSampleRateKhz, frames, paClipOff,
                        audioCallback, NULL);

    if (err != paNoError) {
        printf("PortAudio open error: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return 1;
    }

    if ((err = Pa_StartStream(stream)) != paNoError) {
        printf("PortAudio start error: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    printf("Listening... Press Enter to stop.\n");
    getchar();
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}

void get_host_api_info(const int index) {
    Pa_Initialize();
    const PaHostApiInfo *host_api = Pa_GetHostApiInfo(index);
    printf("Host API: %s\n", host_api->name);
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
        if (device_info->defaultSampleRate > ULTRASONIC_DEVICE_THRESHOLD_HZ && device_info->maxInputChannels > 0) {
            AudioDevice dev;
            snprintf(dev.device_name, sizeof(dev.device_name), "%s", device_info->name);
            dev.maxInputChannels = device_info->maxInputChannels;
            dev.maxOutputChannels = device_info->maxOutputChannels;
            dev.defaultSampleRateKhz = device_info->defaultSampleRate;
            dev.device_id = i;
            available_devices->devices[available_devices->device_count++] = dev;
            available_devices->device_ids[available_devices->device_count] = i;
        }
    }
    Pa_Terminate();
}

void describe_available_ultrasonic_devices(AvailableDevices *available_devices) {
    printf("Found potential ultrasonic devices: %d \n", available_devices->device_count);
    for (int i = 0; i < available_devices->device_count; i++) {
        printf("Device %d: \n", i);
        printf("\tDevice ID: %d \n", available_devices->devices[i].device_id);
        printf("\tName %s\n", available_devices->devices[i].device_name);
        printf("\tSampling rate %.0f Hz\n", available_devices->devices[i].defaultSampleRateKhz);
        printf("\tMax input channels %d\n", available_devices->devices[i].maxInputChannels);
        printf(DEVICE_INFO_SEPARATOR);
    }
}
