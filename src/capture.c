#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/buffer.h"
#include "../include/device.h"
#include "../include/logger.h"

#define ULTRASONIC_DEVICE_THRESHOLD_HZ 48000
#define DEVICE_INFO_SEPARATOR "*****************************\n"

extern bool AUTO;
extern bool LIVE_DEBUG;

static int audioCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData) {
    (void) outputBuffer;
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if (inputBuffer) {
        const int *in = inputBuffer;
        for (unsigned i = 0; i <= framesPerBuffer; i++)
            write_to_psb(userData, (int16_t) in[i]);
    }
    return paContinue;
}

void set_up_input_params(PaStreamParameters *streamParameters, const AudioDevice *audio_device) {
    streamParameters->device = (int) audio_device->device_index;
    streamParameters->channelCount = (int) audio_device->max_input_channels;
    streamParameters->sampleFormat = paInt16;
    streamParameters->suggestedLatency = Pa_GetDeviceInfo((int) audio_device->device_index)->defaultLowInputLatency;
    streamParameters->hostApiSpecificStreamInfo = NULL;
}

void display_stream_settings(AudioDevice *audio_device, const uint32_t frames) {
    if (LIVE_DEBUG) {
        printf("Starting capture from: \033[4m%s\033[0m\n", audio_device->device_name);
        printf("Frame size: \033[4m%d\033[0m samples\n", frames);
        printf("Sampling rate: \033[4m%.0u\033[0m kHz\n", audio_device->default_sample_rate_hz / 1000);
        printf("Input channels: \033[4m%d\033[0m\n", audio_device->max_input_channels);
    }
    log_info("Starting capture from: %s", audio_device->device_name);
    log_info("Buffer size: %d", frames);
    if (!AUTO) {
        printf("\u25b6 Press Enter to start \u21b5 \n");
        (void) getchar();
    }
}

int start_stream(const uint32_t frames, AudioDevice *audio_device, ProcessingSyncBuffer *rb) {
    display_stream_settings(audio_device, frames);
    PaError err;
    PaStream *stream = NULL;

    if ((err = Pa_Initialize()) != paNoError) {
        rb->producer_online = false;
        if (LIVE_DEBUG)
            fprintf(stderr, "PortAudio initialization error: %s\n", Pa_GetErrorText(err));
        log_error("PortAudio initialization error: %s", Pa_GetErrorText(err));
        return 0;
    }

    PaStreamParameters inputParams;
    set_up_input_params(&inputParams, audio_device);
    err = Pa_OpenStream(&stream, &inputParams, NULL, audio_device->default_sample_rate_hz, frames, paClipOff,
                        audioCallback, rb);

    if (err != paNoError) {
        rb->producer_online = false;
        if (LIVE_DEBUG)
            fprintf(stderr, "PortAudio opening stream error: %s\n", Pa_GetErrorText(err));
        log_error("PortAudio opening stream error: %s", Pa_GetErrorText(err));
        Pa_Terminate();
        return 0;
    }

    if ((err = Pa_StartStream(stream)) != paNoError) {
        rb->producer_online = false;
        if (LIVE_DEBUG)
            fprintf(stderr, "PortAudio starting stream error: %s\n", Pa_GetErrorText(err));
        log_error("PortAudio starting stream error: %s", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 0;
    }

    printf("\u276F Listening... Press Enter to stop.\n");
    (void) getchar();
    rb->producer_online = false; // Indicate that the producer is offline
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    if (LIVE_DEBUG) {
        printf("\u2705 Recording session ended. "
               "Processed %lu samples. Duration %lu seconds. "
               "Skipped %u samples (%.5f percent).\n",
               rb->write_count, rb->write_count / audio_device->default_sample_rate_hz, rb->skipped_samples_count,
               (rb->skipped_samples_count / (double) rb->write_count) * 100.0);
    }

    log_info("Processed %lu samples", rb->write_count);
    log_info("Skipped %u samples", rb->skipped_samples_count);

    return 1;
}

void get_host_api_info(const int index) {
    Pa_Initialize();
    const PaHostApiInfo *host_api = Pa_GetHostApiInfo(index);
    fprintf(stdout, "Host API: %s\n", host_api->name);
    Pa_Terminate();
}

void get_audio_devices() {
    Pa_Initialize();
    const int num_devices = Pa_GetDeviceCount();

    for (int i = 0; i < num_devices; i++) {
        const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);
        fprintf(stdout, "Device %d: %s\n", i, device_info->name);
        fprintf(stdout, "Default sample rate: %f\n", device_info->defaultSampleRate);
        fprintf(stdout, "Max output channels: %d\n", device_info->maxOutputChannels);
        fprintf(stdout, "Max input channels: %d\n", device_info->maxInputChannels);
        fprintf(stdout, "Default channel count: %d\n", device_info->maxOutputChannels);
        fprintf(stdout, DEVICE_INFO_SEPARATOR);
    }

    Pa_Terminate();
}

void load_ultrasonic_devices(AvailableDevice *available_devices) {
    Pa_Initialize();
    const int num_devices = Pa_GetDeviceCount();

    available_devices->device_count = 0;

    for (int i = 0; i < num_devices; i++) {
        const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);
        if (device_info->defaultSampleRate > ULTRASONIC_DEVICE_THRESHOLD_HZ && device_info->maxInputChannels > 0) {
            AudioDevice dev;
            snprintf(dev.device_name, sizeof(dev.device_name), "%s", device_info->name);
            dev.max_input_channels = device_info->maxInputChannels;
            dev.max_output_channels = device_info->maxOutputChannels;
            dev.default_sample_rate_hz = (uint32_t) device_info->defaultSampleRate;
            dev.device_index = i;
            available_devices->devices[available_devices->device_count++] = dev;
            available_devices->device_ids[available_devices->device_count] = i;
        }
    }
    Pa_Terminate();
}

void describe_available_ultrasonic_devices(AvailableDevice *available_devices) {
    fprintf(stdout, "Found potential ultrasonic devices: %d \n", available_devices->device_count);
    for (int i = 0; i < available_devices->device_count; i++) {
        fprintf(stdout, "Device #%d: \n", i);
        fprintf(stdout, "\tName: \033[4m%s\033[0m \n", available_devices->devices[i].device_name);
        fprintf(stdout, "\tSampling rate: \033[4m%.0u\033[0m kHz\n",
                available_devices->devices[i].default_sample_rate_hz / 1000);
        fprintf(stdout, "\tMax input channels: \033[4m%d\033[0m\n", available_devices->devices[i].max_input_channels);
        fprintf(stdout, DEVICE_INFO_SEPARATOR);
    }
}
