#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/buffer.h"
#include "src/capture/capture.h"
#include "src/process/process.h"
#include "src/uplink/uplink.h"
#include "src/utilities/utilities.h"

#define ONE_MINUTE 60
#define FIVE_MINUTES 300
#define ONE_HOUR 3600

#define MAX_DURATION_SECONDS (1u << 16)

bool DEBUG = true;
bool AUTO = false;
bool TIMER = false;

uint32_t BUFFER_SIZE;
uint16_t RECORDING_DURATION_SECONDS = 3600;

AppConfig app_config;
AvailableDevice available_devices;

ProcessingSyncBuffer psb;
FeaturesSyncBuffer fsb;

static int input_device_id(const AvailableDevice *devices) {
    int c;
    printf("\u25b6 Please enter device #: \n");
    while ((c = getchar()) != EOF && !isspace(c)) {
        const int n = c - '0';
        if (in_array(devices->device_ids, devices->device_count, n)) {
            return n;
        }
    }
    printf("Not found. Using default # 0\n");
    exit(EXIT_FAILURE);
}

void send_connected_message(MQTTConfig *mqtt_config) {
    char payload[1024];
    SystemSummary summary;
    collect_info(&summary);
    const time_t now = time(NULL);

    snprintf(payload, sizeof(payload), "{\"cpu_name\":\"%s\", \"hostname\":\"%s\", \"timestamp\":%ld}",
             summary.cpu_name, summary.hostname, now);

    send_message(mqtt_config, payload);
}

int main(const int argc, char *argv[]) {
    suppress_alsa_errors();

    int device_idx = -1;
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        help();
        exit(EXIT_SUCCESS);
    }
    if (argc <= 1) {
        fprintf(stderr, "Missing arguments \n");
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
            TIMER = true;
        } else {
            TIMER = true;
            RECORDING_DURATION_SECONDS = requested_duration;
        }
    }

    const char *mqtt_topic = argv[2];
    const char *certs_path = argv[3];
    const char *aws_endpoint = argv[4];

    // MQTTConfig mqtt_config;
    // init_config(mqtt_topic, certs_path, aws_endpoint, &mqtt_config);
    // init_client(&mqtt_config);
    // send_connected_message(&mqtt_config);

    int16_t *p_storage = calloc(BUFFER_SIZE, sizeof(int16_t));
    init_psb(&psb, p_storage); // initialize processing sync buffer

    AudioFeatures *f_storage = calloc(BUFFER_SIZE, sizeof(AudioFeatures));
    init_fsb(&fsb, f_storage); // initialize features sync buffer

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

    pthread_t r_thread;
    ReaderContext reader_context = {&psb, BUFFER_SIZE, audio_device.default_sample_rate_hz};
    pthread_create(&r_thread, NULL, (void *(*) (void *) ) reader_thread, &reader_context);

    start_stream(BUFFER_SIZE, &audio_device, &psb);
    pthread_join(r_thread, NULL);

    cleanup();
    free(p_storage);
    free(f_storage);
    return 0;
}
