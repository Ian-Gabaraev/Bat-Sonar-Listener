#include "uplink.h"

#include <MQTTClient.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>

#define QOS 1
#define TIMEOUT_MS 10000L

#include <openssl/ssl.h>
#include <openssl/err.h>

void test_tls_handshake(const char *host, const char *port,
                        const char *ca, const char *cert, const char *key) {
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

void init_config(const char *mqtt_topic, const char *certs_path,
                 const char *aws_endpoint, MQTTConfig *mqtt_config) {
    snprintf(mqtt_config->aws_endpoint, sizeof(mqtt_config->aws_endpoint),
             "ssl://%s:8883", aws_endpoint);

    snprintf(mqtt_config->ca_path, sizeof(mqtt_config->ca_path),
             "%s/AmazonRootCA1.pem", certs_path);
    snprintf(mqtt_config->cert_path, sizeof(mqtt_config->cert_path),
             "%s/device_cert.pem", certs_path);
    snprintf(mqtt_config->key_path, sizeof(mqtt_config->key_path),
             "%s/private.pem", certs_path);

    snprintf(mqtt_config->mqtt_topic, sizeof(mqtt_config->mqtt_topic), "%s",
             mqtt_topic);

    snprintf(mqtt_config->client_id, sizeof(mqtt_config->client_id),
             "bat-sensor-%d", getpid());

    mqtt_config->qos = QOS;
    mqtt_config->timeout_ms = TIMEOUT_MS;

    printf("Initialized MQTT Config:\n");
    printf("\tAWS Endpoint: %s\n", mqtt_config->aws_endpoint);
    printf("\tMQTT Topic: %s\n", mqtt_config->mqtt_topic);
    printf("\tClient ID: %s\n", mqtt_config->client_id);
}

unsigned init_client(MQTTClient *client, const MQTTConfig *mqtt_config) {
    int rc;

    if ((rc = MQTTClient_create(
                 client, mqtt_config->aws_endpoint, mqtt_config->client_id,
                 MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "MQTTClient_create failed, rc=%d\n", rc);
        return 2;
    }

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;

    if (access(mqtt_config->ca_path, F_OK) == 0) {
        ssl_opts.trustStore = mqtt_config->ca_path;
    } else {
        fprintf(stderr, "CA file not found: %s\n", mqtt_config->ca_path);
    }

    if (access(mqtt_config->cert_path, F_OK) == 0) {
        ssl_opts.CApath = mqtt_config->cert_path;
        ssl_opts.keyStore = mqtt_config->cert_path;
    } else {
        fprintf(stderr, "Cert file not found: %s\n", mqtt_config->ca_path);
    }

    if (access(mqtt_config->key_path, F_OK) == 0) {
        ssl_opts.privateKey = mqtt_config->key_path;
    } else {
        fprintf(stderr, "Private key file not found: %s\n", mqtt_config->key_path);
    }

    printf("Trust Store: %s\nKey Store: %s\nPrivate Key: %s\n",
           ssl_opts.trustStore, ssl_opts.keyStore, ssl_opts.privateKey);

    ssl_opts.enableServerCertAuth = 1;
    ssl_opts.sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    ssl_opts.struct_version = 3;

    conn_opts.keepAliveInterval = 30;
    conn_opts.cleansession = 1;
    conn_opts.connectTimeout = 10;
    conn_opts.ssl = &ssl_opts;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "MQTTClient_connect failed, rc=%d\n", rc);
        MQTTClient_destroy(client);
        return 3;
    }
    return rc;
}

void send_message(MQTTClient *client, MQTTConfig *config, char *payload) {
    int rc;

    MQTTClient_message mqttMessage = MQTTClient_message_initializer;
    mqttMessage.payload = (void *) payload;
    mqttMessage.payloadlen = (int) strlen(payload);
    mqttMessage.qos = QOS;
    mqttMessage.retained = 0;

    MQTTClient_deliveryToken token;
    if ((rc = MQTTClient_publishMessage(client, config->mqtt_topic, &mqttMessage,
                                        &token)) != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "MQTTClient_publishMessage failed, rc=%d\n", rc);
    }

    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT_MS);
    if (rc != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "waitForCompletion failed, rc=%d\n", rc);
    } else {
        printf("Published to topic '%s': %s\n", config->mqtt_topic, payload);
    }
}

void cleanup(MQTTClient *client) {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(client);
}