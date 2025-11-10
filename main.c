#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/config.h"
#include "include/buffer.h"
#include "include/logger.h"
#include "include/process.h"
#include "include/uplink.h"
#include "include/utilities.h"

#define ONE_MINUTE 60
#define FIVE_MINUTES 300
#define ONE_HOUR 3600

#define MAX_DURATION_SECONDS (1u << 16) // 2^16

#define LOG_FILE_NAME "sonar.log"

bool LIVE_DEBUG = true;
bool AUTO = false;
bool TIMER = false;

uint32_t BUFFER_SIZE = 128000;
uint16_t RECORDING_DURATION_SECONDS = 3600;

AppConfig app_config;
AvailableDevice available_devices;

ProcessingSyncBuffer psb;
FeaturesSyncBuffer fsb;

void process_args(UserSettings *settings, const int argc, char *argv[]) {
    *settings = (UserSettings) { .ping = false, ._auto = false, .timer = false };

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        help();
        exit(EXIT_SUCCESS);
    }

    if (argc < 5) {
        fprintf(stderr, "Missing mandatory arguments \n");
        fprintf(stderr, "sonar -lm <mqtt topic> <certificates path> <aws endpoint> [buffer size] [device index] [duration] [ping] \n");
        log_error("Missing mandatory arguments");
        log_error("Call using format: sonar -lm <mqtt topic> <certificates path> <aws endpoint> [buffer size] [device index] [duration] [ping]");
        exit(EXIT_FAILURE);
    }

    if (argc >= 6) {
        settings->buffer_size = (uint32_t) strtoul(argv[5], NULL, 10); // convert str to unsigned long
        BUFFER_SIZE = settings->buffer_size;
    }

    if (argc >= 7) {
        settings->device_idx = (int) strtol(argv[6], NULL, 10); // convert str to long
        settings->_auto = settings->device_idx >= 0 ? true : false;
        AUTO = settings->_auto;
    }

    if (argc >= 8) {
        settings->requested_duration = (uint16_t) strtoul(argv[7], NULL, 10);
        if (settings->requested_duration > MAX_DURATION_SECONDS-1) {
            fprintf(stderr, "Duration exceeds limit %u", MAX_DURATION_SECONDS - 1);
            log_warning("Duration exceeds set limit %u", MAX_DURATION_SECONDS - 1);
            settings->timer = true;
            TIMER = true;
        }
        if (settings->requested_duration < MAX_DURATION_SECONDS-1 && settings->requested_duration > 0) {
            log_info("Set recording duration to %u seconds", settings->requested_duration);
            settings->timer = true;
            TIMER = true;
            RECORDING_DURATION_SECONDS = settings->requested_duration;
        }
    }

    if (argc >= 9) {
        settings->ping = (int) strtol(argv[8], NULL, 10) == 1 ? true : false;
    }

    settings->mqtt_topic = argv[2];
    settings->certs_path = argv[3];
    settings->aws_endpoint = argv[4];
}

void select_device(const UserSettings *user_settings, AudioDevice *audio_device) {

    load_ultrasonic_devices(&available_devices);
    if (!user_settings->_auto) {
        describe_available_ultrasonic_devices(&available_devices);
    }

    if (user_settings->device_idx >= 0) {
        *audio_device = available_devices.devices[user_settings->device_idx];
    } else {
        const int device_id = input_device_id(&available_devices);
        (void) getchar();
        *audio_device = available_devices.devices[device_id];
    }
    log_info("Device selected");
}

int main(const int argc, char *argv[]) {
    suppress_alsa_errors();
    logger_init(LOG_FILE_NAME);
    log_info("App launched");

    UserSettings user_settings;
    process_args(&user_settings, argc, argv);

    MQTTConfig mqtt_config;
    init_config(user_settings.mqtt_topic, user_settings.certs_path, user_settings.aws_endpoint, &mqtt_config);
    init_client(&mqtt_config);

    if (user_settings.ping) send_connected_message(&mqtt_config);

    int16_t *p_storage = calloc(user_settings.buffer_size, sizeof(int16_t));
    init_psb(&psb, p_storage); // initialize processing sync buffer

    AudioFeatures *f_storage = calloc(user_settings.buffer_size, sizeof(AudioFeatures));
    init_fsb(&fsb, f_storage); // initialize features sync buffer

    AudioDevice audio_device;
    select_device(&user_settings, &audio_device);

    ProcessContext process_context = {&psb, &fsb, BUFFER_SIZE, audio_device.default_sample_rate_hz};
    RelayContext relay_context = {&mqtt_config, &fsb};

    pthread_t processing_thread ;
    pthread_t uplink_thread;
    pthread_create(&processing_thread, NULL, (void *(*) (void *) ) reader_thread, &process_context);
    pthread_create(&uplink_thread, NULL, (void *(*) (void *) ) relay_thread, &relay_context);

    start_stream(BUFFER_SIZE, &audio_device, &psb, &fsb);

    pthread_join(processing_thread, NULL);
    pthread_join(uplink_thread, NULL);

    paho_cleanup();
    free(p_storage);
    free(f_storage);
    logger_close();

    return 0;
}
