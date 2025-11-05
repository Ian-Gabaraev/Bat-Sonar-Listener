#pragma once

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
