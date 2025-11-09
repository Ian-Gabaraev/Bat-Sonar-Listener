#include <MQTTClient.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/logger.h"
#include "../include/system.h"
#include "../include/utilities.h"
#include "../include/uplink.h"

#define QOS 0
#define TIMEOUT_MS 10000L

#include <openssl/err.h>
#include <stdbool.h>

extern bool LIVE_DEBUG;

static MQTTClient client;

void send_message(MQTTConfig *config, char *payload) {
    int rc;

    MQTTClient_message mqttMessage = MQTTClient_message_initializer;
    mqttMessage.payload = (void *) payload;
    mqttMessage.payloadlen = (int) strlen(payload);
    mqttMessage.qos = QOS;
    mqttMessage.retained = 0;

    MQTTClient_deliveryToken token;
    if ((rc = MQTTClient_publishMessage(client, config->mqtt_topic, &mqttMessage, &token)) != MQTTCLIENT_SUCCESS) {
        if (LIVE_DEBUG)
            fprintf(stderr, "MQTTClient_publishMessage failed, rc=%d\n", rc);
        log_error("MQTTClient_publishMessage failed");
    }

    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT_MS);
    if (rc != MQTTCLIENT_SUCCESS) {
        if (LIVE_DEBUG)
            fprintf(stderr, "waitForCompletion failed, rc=%d\n", rc);
    } else {
        if (LIVE_DEBUG)
            fprintf(stdout, "Published to topic '%s': %s\n", config->mqtt_topic, payload);
        log_info("Published to MQTT topic '%s': %s", config->mqtt_topic, payload);
    }
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

void init_config(const char *mqtt_topic, const char *certs_path, const char *aws_endpoint, MQTTConfig *mqtt_config) {
    snprintf(mqtt_config->aws_endpoint, sizeof(mqtt_config->aws_endpoint), "ssl://%s:8883", aws_endpoint);

    snprintf(mqtt_config->ca_path, sizeof(mqtt_config->ca_path), "%s/AmazonRootCA1.pem", certs_path);
    snprintf(mqtt_config->cert_path, sizeof(mqtt_config->cert_path), "%s/device_cert.pem", certs_path);
    snprintf(mqtt_config->key_path, sizeof(mqtt_config->key_path), "%s/private.pem", certs_path);

    snprintf(mqtt_config->mqtt_topic, sizeof(mqtt_config->mqtt_topic), "%s", mqtt_topic);

    snprintf(mqtt_config->client_id, sizeof(mqtt_config->client_id), "bat-sensor-%d", getpid());

    mqtt_config->qos = QOS;
    mqtt_config->timeout_ms = TIMEOUT_MS;

    if (LIVE_DEBUG) {
        printf("Initialized MQTT Config:\n");
        printf("\tAWS Endpoint: %s\n", mqtt_config->aws_endpoint);
        printf("\tMQTT Topic: %s\n", mqtt_config->mqtt_topic);
        printf("\tClient ID: %s\n", mqtt_config->client_id);
    }
    log_info("Initialized MQTT Config");
    log_info("AWS Endpoint: %s", mqtt_config->aws_endpoint);
    log_info("MQTT Topic: %s", mqtt_config->mqtt_topic);
    log_info("Client ID: %s", mqtt_config->client_id);
}

unsigned init_client(const MQTTConfig *mqtt_config) {
    int rc;

    if ((rc = MQTTClient_create(&client, mqtt_config->aws_endpoint, mqtt_config->client_id, MQTTCLIENT_PERSISTENCE_NONE,
                                NULL)) != MQTTCLIENT_SUCCESS) {
        if (LIVE_DEBUG)
            fprintf(stderr, "create rc=%d\n", rc);
        return 1;
    }

    MQTTClient_connectOptions conn = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl = MQTTClient_SSLOptions_initializer;

    if (access(mqtt_config->ca_path, F_OK) == 0) {
        ssl.trustStore = mqtt_config->ca_path;
    } else {
        if (LIVE_DEBUG)
            fprintf(stderr, "CA file not found: %s\n", mqtt_config->ca_path);
        log_error("CA filed not found: %s", mqtt_config->ca_path);
        return 0;
    }

    if (access(mqtt_config->cert_path, F_OK) == 0) {
        ssl.CApath = mqtt_config->cert_path;
        ssl.keyStore = mqtt_config->cert_path;
    } else {
        if (LIVE_DEBUG)
            fprintf(stderr, "Cert file not found: %s\n", mqtt_config->ca_path);
        log_error("Certificate file not found: %s", mqtt_config->ca_path);
        return 0;
    }

    if (access(mqtt_config->key_path, F_OK) == 0) {
        ssl.privateKey = mqtt_config->key_path;
    } else {
        if (LIVE_DEBUG)
            fprintf(stderr, "Private key file not found: %s\n", mqtt_config->key_path);
        log_error("Private key not found: %s", mqtt_config->key_path);
        return 0;
    }

    ssl.enableServerCertAuth = 1;
    ssl.sslVersion = MQTT_SSL_VERSION_TLS_1_2;

    ssl.enabledCipherSuites = "ALL:@SECLEVEL=1";

    conn.ssl = &ssl;
    conn.keepAliveInterval = 30;
    conn.cleansession = 1;
    conn.connectTimeout = 10;
    conn.MQTTVersion = MQTTVERSION_3_1_1;

    rc = MQTTClient_connect(client, &conn);
    if (rc != MQTTCLIENT_SUCCESS) {
        if (LIVE_DEBUG)
            fprintf(stderr, "connect rc=%d\n", rc);
        MQTTClient_destroy(client);
        return 2;
    }
    if (LIVE_DEBUG)
        fprintf(stdout, "Connected to AWS IoT Core at %s\n", mqtt_config->aws_endpoint);
    log_info("Connected to AWS IoT Core at %s", mqtt_config->aws_endpoint);
    return 1;
}

AudioFeatures *read_from_fsb(FeaturesSyncBuffer *buffer) {
    if (buffer->reading_at >= buffer->writing_at) {
        return NULL;
    }
    AudioFeatures *value = &buffer->storage[buffer->reading_at];
    buffer->reading_at++;
    return value;
}

void *relay_thread(const RelayContext *rc) {
    while (true) {
        const AudioFeatures *features = read_from_fsb(rc->fsb);
        if (features == NULL) continue;
        char payload[1024];
        const time_t now = time(NULL);

        snprintf(payload, sizeof(payload), "{\"dominant_frequency\":\"%f\", \"rms\":\"%f\", \"timestamp\":%ld}",
                 features->dominant_freq, features->rms, now);
        send_message(rc->mqtt_config, payload);
    }
}

void cleanup() {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}
