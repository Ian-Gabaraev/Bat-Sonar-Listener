#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/buffer.h"
#include "include/logger.h"
#include "include/process.h"
#include "include/uplink.h"
#include "include/utilities.h"

#define ONE_MINUTE 60
#define FIVE_MINUTES 300
#define ONE_HOUR 3600

#define MAX_DURATION_SECONDS (1u << 16) // 2^16

bool LIVE_DEBUG = true;
bool AUTO = false;
bool TIMER = false;
bool PING_IOT_CORE = false;

uint32_t BUFFER_SIZE;
uint16_t RECORDING_DURATION_SECONDS = 3600;

AppConfig app_config;
AvailableDevice available_devices;

ProcessingSyncBuffer psb;
FeaturesSyncBuffer fsb;

int main(const int argc, char *argv[]) {
    suppress_alsa_errors();
    logger_init("sonar.log");

    log_info("App launched");

    int device_idx = -1;
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        help();
        exit(EXIT_SUCCESS);
    }
    if (argc <= 1) {
        fprintf(stderr, "Missing arguments \n");
        log_error("Missing arguments");
        exit(EXIT_FAILURE);
    }

    if (argc >= 5) {
        BUFFER_SIZE = (uint32_t) strtoul(argv[5], NULL, 10); // convert str to unsigned long
    }
    if (argc >= 6) {
        device_idx = (int) strtol(argv[6], NULL, 10); // convert str to long
        AUTO = device_idx >= 0 ? true : false;
    }
    if (argc >= 7) {
        const uint16_t requested_duration = (uint16_t) strtoul(argv[7], NULL, 10);
        if (requested_duration >= MAX_DURATION_SECONDS - 1) {
            fprintf(stderr, "Duration exceeds limit %u", MAX_DURATION_SECONDS - 1);
            log_warning("Duration exceeds set limit %u", MAX_DURATION_SECONDS - 1);
            TIMER = true;
        } else {
            TIMER = true;
            RECORDING_DURATION_SECONDS = requested_duration;
        }
    }

    const char *mqtt_topic = argv[2];
    const char *certs_path = argv[3];
    const char *aws_endpoint = argv[4];

    MQTTConfig mqtt_config;
    init_config(mqtt_topic, certs_path, aws_endpoint, &mqtt_config);
    init_client(&mqtt_config);

    if (PING_IOT_CORE) {
        send_connected_message(&mqtt_config);
        log_info("AWS IoT Core ping sent");
    }

    int16_t *p_storage = calloc(BUFFER_SIZE, sizeof(int16_t));
    init_psb(&psb, p_storage); // initialize processing sync buffer

    AudioFeatures *f_storage = calloc(BUFFER_SIZE, sizeof(AudioFeatures));
    init_fsb(&fsb, f_storage); // initialize features sync buffer

    log_info("Buffers initialized");

    load_ultrasonic_devices(&available_devices);
    if (!AUTO) {
        describe_available_ultrasonic_devices(&available_devices);
    }
    AudioDevice audio_device;

    if (device_idx >= 0) {
        audio_device = available_devices.devices[device_idx];
    } else {
        const int device_id = input_device_id(&available_devices);
        (void) getchar();
        audio_device = available_devices.devices[device_id];
    }

    log_info("Device selected");

    pthread_t processing_thread = 0;
    pthread_t uplink_thread = 0;

    ProcessContext process_context = {&psb, &fsb, BUFFER_SIZE, audio_device.default_sample_rate_hz};
    RelayContext relay_context = {&mqtt_config, &fsb};

    pthread_create(&processing_thread, NULL, (void *(*) (void *) ) reader_thread, &process_context);
    pthread_create(&uplink_thread, NULL, (void *(*) (void *) ) relay_thread, &relay_context);

    log_info("Threads started");

    start_stream(BUFFER_SIZE, &audio_device, &psb);

    pthread_join(processing_thread, NULL);
    pthread_join(uplink_thread, NULL);

    cleanup();
    free(p_storage);
    free(f_storage);
    log_info("Cleaned up resources");
    log_info("App exited");
    logger_close();
    return 0;
}
