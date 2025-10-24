#include <stdio.h>
#include "src/config/config.h"
#include "src/capture/capture.h"

void suppress_alsa_errors() {
    freopen("/dev/null", "w", stderr);
}

AppConfig app_config;

int main(void) {
    suppress_alsa_errors();
    load_config("config.json", &app_config);
    // printf("Loaded config:\n");
    // printf("Device Name: %s\n", app_config.device_name);
    // printf("Sample Rate: %d\n", app_config.sample_rate);
    // printf("Channel Count: %d\n", app_config.channel_count);
    // printf("Frames Per Buffer: %d\n", app_config.frames_per_buffer);
    // printf("MQTT Topic: %s\n", app_config.mqtt_topic);
    // printf("AWS Endpoint: %s\n", app_config.aws_endpoint);

    get_ultrasonic_devices();

    return 0;
}