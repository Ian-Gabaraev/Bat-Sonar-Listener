#include "uplink.h"

#include <MQTTClient.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>

#define QOS 1
#define TIMEOUT_MS 10000L

#include <openssl/err.h>
#include <openssl/ssl.h>

static MQTTClient client;

void test_tls_handshake(const char *host, const char *port, const char *ca, const char *cert, const char *key) {
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
    if (!SSL_CTX_load_verify_locations(ctx, ca, NULL))
        perror("CA load fail");
    if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0)
        perror("Cert load fail");
    if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0)
        perror("Key load fail");

    BIO *bio;
    char target[256];
    snprintf(target, sizeof(target), "%s:%s", host, port);
    bio = BIO_new_ssl_connect(ctx);
    BIO_set_conn_hostname(bio, target);
    if (BIO_do_connect(bio) <= 0)
        ERR_print_errors_fp(stderr);
    else
        printf("TLS connect OK ✅\n");

    BIO_free_all(bio);
    SSL_CTX_free(ctx);
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

    printf("Initialized MQTT Config:\n");
    printf("\tAWS Endpoint: %s\n", mqtt_config->aws_endpoint);
    printf("\tMQTT Topic: %s\n", mqtt_config->mqtt_topic);
    printf("\tClient ID: %s\n", mqtt_config->client_id);
}

unsigned init_client(const MQTTConfig *mqtt_config) {
    int rc;

    if ((rc = MQTTClient_create(&client, mqtt_config->aws_endpoint, mqtt_config->client_id, MQTTCLIENT_PERSISTENCE_NONE,
                                NULL)) != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "create rc=%d\n", rc);
        return 1;
    }

    MQTTClient_connectOptions conn = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl = MQTTClient_SSLOptions_initializer;

    if (access(mqtt_config->ca_path, F_OK) == 0) {
        ssl.trustStore = mqtt_config->ca_path;
    } else {
        fprintf(stderr, "CA file not found: %s\n", mqtt_config->ca_path);
        return 0;
    }

    if (access(mqtt_config->cert_path, F_OK) == 0) {
        ssl.CApath = mqtt_config->cert_path;
        ssl.keyStore = mqtt_config->cert_path;
    } else {
        fprintf(stderr, "Cert file not found: %s\n", mqtt_config->ca_path);
        return 0;
    }

    if (access(mqtt_config->key_path, F_OK) == 0) {
        ssl.privateKey = mqtt_config->key_path;
    } else {
        fprintf(stderr, "Private key file not found: %s\n", mqtt_config->key_path);
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
        fprintf(stderr, "connect rc=%d\n", rc);
        MQTTClient_destroy(client);
        return 2;
    }
    return 1;
}

void send_message(MQTTConfig *config, char *payload) {
    int rc;

    MQTTClient_message mqttMessage = MQTTClient_message_initializer;
    mqttMessage.payload = (void *) payload;
    mqttMessage.payloadlen = (int) strlen(payload);
    mqttMessage.qos = QOS;
    mqttMessage.retained = 0;

    MQTTClient_deliveryToken token;
    if ((rc = MQTTClient_publishMessage(client, config->mqtt_topic, &mqttMessage, &token)) != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "MQTTClient_publishMessage failed, rc=%d\n", rc);
    }

    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT_MS);
    if (rc != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "waitForCompletion failed, rc=%d\n", rc);
    } else {
        printf("Published to topic '%s': %s\n", config->mqtt_topic, payload);
    }
}

void cleanup() {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}
