#include <stdio.h>
#include "src/config/config.h"
#include "src/capture/capture.h"

void suppress_alsa_errors() {
    freopen("/dev/null", "w", stderr);
}

AppConfig app_config;
AvailableDevices available_devices;

int main(void) {
    suppress_alsa_errors();
    load_config("config.json", &app_config);
    load_ultrasonic_devices(&available_devices);
    describe_available_ultrasonic_devices(&available_devices);

    return 0;
}