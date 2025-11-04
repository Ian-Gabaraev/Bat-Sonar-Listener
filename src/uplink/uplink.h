#pragma once
#include <MQTTClient.h>

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

void init_config(const char *mqtt_topic, const char *certs_path, const char *aws_endpoint, MQTTConfig *mqtt_config);
unsigned init_client(const MQTTConfig *mqtt_config);
void send_message(MQTTConfig *config, char *payload);
void cleanup();
