#pragma once
#include "../../include/config.h"

void init_config(const char *mqtt_topic, const char *certs_path, const char *aws_endpoint, MQTTConfig *mqtt_config);
unsigned init_client(const MQTTConfig *mqtt_config);
void send_message(MQTTConfig *config, char *payload);
void cleanup();
