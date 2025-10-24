#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "cjson/cJSON.h"

int load_config(const char *filename, AppConfig *config) {
  FILE *file = fopen(filename, "r");

  if (!file) {
    printf("Config file not found\n");
    return -1;
  }

  fseek(file, 0, SEEK_END);
  const long size = ftell(file);
  rewind(file);

  // Allocate memory to hold the file contents
  char *json_data = malloc(size + 1);  // +1 for null terminator

  if (!json_data) {
    fclose(file);
    fprintf(stderr, "Memory error\n");
    return -1;
  }

  fread(json_data, 1, size, file);
  json_data[size] = '\0';  // Null-terminate the string
  fclose(file);

  cJSON *json = cJSON_Parse(json_data);

  if (!json) {
    printf("Error loading configuration\n");
    free(json_data);
    return -1;
  }

  const char *device_name =
      cJSON_GetObjectItem(json, "device_name")->valuestring;
  const char *mqtt_topic = cJSON_GetObjectItem(json, "mqtt_topic")->valuestring;
  const char *aws_endpoint =
      cJSON_GetObjectItem(json, "aws_endpoint")->valuestring;

  config->sample_rate = cJSON_GetObjectItem(json, "sample_rate")->valueint;
  config->channel_count = cJSON_GetObjectItem(json, "channel_count")->valueint;
  config->frames_per_buffer =
      cJSON_GetObjectItem(json, "frames_per_buffer")->valueint;

  snprintf(config->device_name, sizeof(config->device_name), "%s", device_name);
  snprintf(config->mqtt_topic, sizeof(config->mqtt_topic), "%s", mqtt_topic);
  snprintf(config->aws_endpoint, sizeof(config->aws_endpoint), "%s",
           aws_endpoint);

  cJSON_Delete(json);  // Free the cJSON object
  free(json_data);     // Free the JSON data buffer
  return 0;
}
