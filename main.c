#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/buffer/synchronous_single_buffer.h"
#include "src/capture/capture.h"
#include "src/config/config.h"
#include "src/utilities/utilities.h"

AppConfig app_config;
AvailableDevice available_devices;

int input_device_id(const AvailableDevice* devices) {
  char c;
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

int main(void) {
  suppress_alsa_errors();
  int* storage = malloc(SYNCHRONOUS_SINGULAR_BUFFER_SIZE);
  buffer.storage = storage;
  init_buffer(&buffer, storage);

  load_config("config.json", &app_config);

  load_ultrasonic_devices(&available_devices);
  describe_available_ultrasonic_devices(&available_devices);

  const int device_id = input_device_id(&available_devices);
  AudioDevice audio_device = available_devices.devices[device_id];

  pthread_t read_thread;
  // pthread_create(&gui_thread, NULL, (void *(*) (void *) ) run_gui, &gui_ctx);

  start_stream(app_config.frames_per_buffer, &audio_device, &buffer);
  // pthread_join(gui_thread, NULL);
  free(storage);
  return 0;
}
