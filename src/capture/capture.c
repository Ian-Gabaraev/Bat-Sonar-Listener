#include "capture.h"

#include <portaudio.h>
#include <stdio.h>
#include <string.h>

#include "../buffer/synchronous_single_buffer.h"

#define RING_BUFFER_SIZE 4096

#define ULTRASONIC_DEVICE_THRESHOLD_HZ 48000
#define DEVICE_INFO_SEPARATOR "*****************************\n"

static int audioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData) {
  (void)outputBuffer;
  (void)timeInfo;
  (void)statusFlags;
  (void)userData;

  if (inputBuffer) {
    const int *in = inputBuffer;
    for (unsigned i = 0; i <= framesPerBuffer; i++)
      write_to_buffer(userData, in[i]);
  }
  return paContinue;
}

void set_up_input_params(PaStreamParameters *streamParameters,
                         const AudioDevice *audio_device) {
  streamParameters->device = audio_device->device_index;
  streamParameters->channelCount = audio_device->max_input_channels;
  streamParameters->sampleFormat = paInt16;
  streamParameters->suggestedLatency =
      Pa_GetDeviceInfo(audio_device->device_index)->defaultLowInputLatency;
  streamParameters->hostApiSpecificStreamInfo = NULL;
}

void display_stream_settings(AudioDevice *audio_device, const int frames) {
  printf("Starting capture from: \033[4m%s\033[0m\n",
         audio_device->device_name);
  printf("Sample size: \033[4m%d\033[0m\n", frames);
  printf("Input channels: \033[4m%d\033[0m\n",
         audio_device->max_input_channels);
  printf("\u25b6 Press Enter to start \u21b5 \n");
  getchar();
}

int start_stream(const int frames, AudioDevice *audio_device,
                 SynchronousSingleBuffer *rb) {
  display_stream_settings(audio_device, frames);
  PaError err;
  PaStream *stream = NULL;

  if ((err = Pa_Initialize()) != paNoError) {
    printf("PortAudio init error: %s\n", Pa_GetErrorText(err));
    return 1;
  }

  PaStreamParameters inputParams;
  set_up_input_params(&inputParams, audio_device);
  err = Pa_OpenStream(&stream, &inputParams, NULL,
                      audio_device->default_sample_rate_hz, frames, paClipOff,
                      audioCallback, rb);

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

  printf("\u276F Listening... Press Enter to stop.\n");
  getchar();
  rb->producer_online = false; // Indicate that the producer is offline
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  printf("\u2705 Recording session ended. Processed %d samples.\n", rb->write_count);

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

void load_ultrasonic_devices(AvailableDevice *available_devices) {
  Pa_Initialize();
  const int num_devices = Pa_GetDeviceCount();

  available_devices->device_count = 0;

  for (int i = 0; i < num_devices; i++) {
    const PaDeviceInfo *device_info = Pa_GetDeviceInfo(i);
    if (device_info->defaultSampleRate > ULTRASONIC_DEVICE_THRESHOLD_HZ &&
        device_info->maxInputChannels > 0) {
      AudioDevice dev;
      snprintf(dev.device_name, sizeof(dev.device_name), "%s",
               device_info->name);
      dev.max_input_channels = device_info->maxInputChannels;
      dev.max_output_channels = device_info->maxOutputChannels;
      dev.default_sample_rate_hz = device_info->defaultSampleRate;
      dev.device_index = i;
      available_devices->devices[available_devices->device_count++] = dev;
      available_devices->device_ids[available_devices->device_count] = i;
    }
  }
  Pa_Terminate();
}

void describe_available_ultrasonic_devices(AvailableDevice *available_devices) {
  printf("Found potential ultrasonic devices: %d \n",
         available_devices->device_count);
  for (int i = 0; i < available_devices->device_count; i++) {
    printf("Device #%d: \n", i);
    printf("\tName: \033[4m%s\033[0m \n",
           available_devices->devices[i].device_name);
    printf("\tSampling rate: \033[4m%.0f\033[0m kHz\n",
           available_devices->devices[i].default_sample_rate_hz / 1000);
    printf("\tMax input channels: \033[4m%d\033[0m\n",
           available_devices->devices[i].max_input_channels);
    printf(DEVICE_INFO_SEPARATOR);
  }
}
