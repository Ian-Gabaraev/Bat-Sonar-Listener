#pragma once

#include <stddef.h>


#include "config.h"
#include "device.h"
#include "system.h"

int in_array(const unsigned *arr, size_t len, int value);
void suppress_alsa_errors();
void help();
void collect_info(SystemSummary *summary);
int input_device_id(const AvailableDevice *devices);
void send_connected_message(MQTTConfig *mqtt_config);
void set_hostname(SystemSummary *_s);
void set_cpu_model(SystemSummary *_s);
void set_ram(SystemSummary *_s);
