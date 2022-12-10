// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <cstdio>
#include <cstdlib>

#include "mosquitto.h"

// Azure IoT Hub CONNECTION information to complete
#define IOTHUBNAME      "{iothub_name}"
#define DEVICEID        "{device_id}"
#define SAS_TOKEN       "{sas_token}"
#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"

// Computed Host Username and Topic
#define HOST          IOTHUBNAME ".azure-devices.net"
#define PORT          8883
#define USERNAME      HOST "/" DEVICEID "/?api-version=2020-09-30"
#define DEVICEMESSAGE "devices/" DEVICEID "/messages/#"

void connect_callback(struct mosquitto* mosq, void* obj, int result)
{
    printf("Connect callback returned result: %s\r\n", mosquitto_strerror(result));
    if (result == MOSQ_ERR_CONN_REFUSED)
    {
        printf("Connection refused. Please check your SAS Token, expired ?\r\n");
    }
    else
    {
        printf("Waiting for C2D messages...\r\n");
    }
}

void disconnect_callback(struct mosquitto* mosq, void* obj, int rc)
{
    printf("Disconnect callback %d : %s", rc, mosquitto_strerror(rc));
}

void message_callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message)
{
    printf("C2D message '%.*s' for topic '%s'\r\n", message->payloadlen, (char*)message->payload, message->topic);

    bool match = 0;
    mosquitto_topic_matches_sub(DEVICEMESSAGE, message->topic, &match);

    if (match)
    {
        printf("Got message for " DEVICEMESSAGE " topic\r\n");
    }
}

int mosquitto_error(int rc, const char* message = NULL)
{
    printf("Error: %s\r\n", mosquitto_strerror(rc));

    if (message != NULL)
    {
        printf("%s\r\n", message);
    }

    mosquitto_lib_cleanup();
    return rc;
}

int main(int argc, char* argv[])
{
    int rc;

    // init the mosquitto lib
    mosquitto_lib_init();

    // create a mosquito object
    struct mosquitto* mosq = mosquitto_new(DEVICEID, false, NULL);

    // add callback fuctions
    mosquitto_connect_callback_set(mosq, connect_callback);
    mosquitto_message_callback_set(mosq, message_callback);
    mosquitto_disconnect_callback_set(mosq, disconnect_callback);

    // specify mosquitto username, password and options
    mosquitto_username_pw_set(mosq, USERNAME, SAS_TOKEN);

    // specify the certificate to use
    mosquitto_tls_set(mosq, CERTIFICATEFILE, NULL, NULL, NULL, NULL);

    // specify the mqtt version to use
    int option = MQTT_PROTOCOL_V311;
    rc         = mosquitto_opts_set(mosq, MOSQ_OPT_PROTOCOL_VERSION, &option);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc, "Error: opts_set protocol version");
    }

    // connect
    rc = mosquitto_connect(mosq, HOST, PORT, 10);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }

    rc = mosquitto_subscribe(mosq, NULL, "#", 0);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }

    rc = mosquitto_loop_forever(mosq, -1, 1);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }

    mosquitto_lib_cleanup();
    return 0;
}
