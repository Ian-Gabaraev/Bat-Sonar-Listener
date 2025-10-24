#include <ctype.h>
#include <stdio.h>

#include "src/capture/capture.h"
#include "src/config/config.h"

void suppress_alsa_errors() {
  if (!freopen("/dev/null", "w", stderr)) {
    perror("freopen failed");
  }
}

AppConfig app_config;
AvailableDevices available_devices;

int in_array(const int *arr, size_t len, int value) {
  for (size_t i = 0; i < len; i++) {
    if (arr[i] == value) return 1;
  }
  return 0;
}

int input_device_id(const AvailableDevices *devices) {
  char c;
  printf("Please enter device id: \n");
  while ((c = getchar()) != EOF && !isspace(c)) {
    const int n = c - '0';
    if (in_array(devices->device_ids, devices->device_count, n)) {
      printf("Found: %d\n", n);
      return n;
    }
  }
  printf("Not found. Using default ID 0\n");
  return 0;
}

int main(void) {
  suppress_alsa_errors();
  load_config("config.json", &app_config);
  load_ultrasonic_devices(&available_devices);
  describe_available_ultrasonic_devices(&available_devices);
  const int device_id = input_device_id(&available_devices);
  getchar();
  start_stream(app_config.frames_per_buffer,
               &available_devices.devices[device_id]);
  return 0;
}