#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char mqtt_topic[512];
    char certs_path[512];
    char aws_endpoint[512];
    char ca_path[512];
    char cert_path[512];
    char key_path[512];
    unsigned qos;
    int timeout_ms;
    char client_id[64];
} MQTTConfig;

typedef struct {
    char device_name[128];
    int sample_rate;
    int channel_count;
    int frames_per_buffer;
    char mqtt_topic[128];
    char aws_endpoint[256];
} AppConfig;

typedef struct {
    int device_idx;
    char *mqtt_topic;
    char *certs_path;
    char *aws_endpoint;
    uint16_t requested_duration;
    uint32_t buffer_size;
    bool timer;
    bool _auto;
    bool ping;
} UserSettings;