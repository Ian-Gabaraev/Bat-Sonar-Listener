//
// Created by ian on 18/10/2025.
//

#ifndef BAT_SONAR_LISTENER_CONFIG_H
#define BAT_SONAR_LISTENER_CONFIG_H

#pragma once

typedef struct {
    char device_name[128];
    int sample_rate;
    int channel_count;
    int frames_per_buffer;
    char mqtt_topic[128];
    char aws_endpoint[256];
} AppConfig;

int load_config(const char *filename, AppConfig *config);

#endif //BAT_SONAR_LISTENER_CONFIG_H