#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/buffer/synchronous_single_buffer.h"
#include "src/capture/capture.h"
#include "src/config/config.h"
#include "src/process/process.h"
#include "src/utilities/utilities.h"

AppConfig app_config;
AvailableDevice available_devices;

int input_device_id(const AvailableDevice* devices) {
  int c;
  printf("\u25b6 Please enter device #: \n");
  while ((c = getchar()) != EOF && !isspace(c)) {
    const int n = c - '0';
    if (in_array(devices->device_ids, devices->device_count, n)) {
      return n;
    }
  }
  printf("Not found. Using default # 0\n");
  return 0;
}

SynchronousSingleBuffer buffer;

int main(const int argc, char *argv[]) {
  if (argc > 1) {
    const char *mqtt_topic = argv[2];
    const char *certs_path = argv[3];
    printf("MQTT Topic %s\n", mqtt_topic);
  }
  suppress_alsa_errors();
  int16_t* storage = calloc(SYNCHRONOUS_SINGULAR_BUFFER_SIZE, sizeof(int16_t));
  buffer.storage = storage;
  init_buffer(&buffer, storage);

  load_config("config.json", &app_config);

  load_ultrasonic_devices(&available_devices);
  describe_available_ultrasonic_devices(&available_devices);

  const int device_id = input_device_id(&available_devices);
  (void)getchar();
  AudioDevice audio_device = available_devices.devices[device_id];

  pthread_t r_thread;
  ReaderContext reader_context = {&buffer, SYNCHRONOUS_SINGULAR_BUFFER_SIZE,
                                  audio_device.default_sample_rate_hz};
  pthread_create(&r_thread, NULL, (void* (*)(void*))reader_thread,
                 &reader_context);

  start_stream(SYNCHRONOUS_SINGULAR_BUFFER_SIZE, &audio_device, &buffer);
  pthread_join(r_thread, NULL);

  free(storage);
  return 0;
}
