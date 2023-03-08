// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mosquitto.h"

#include "config.h"

#define HOST     IOTHUBNAME ".azure-devices.net"
#define PORT     8883
#define USERNAME HOST "/" DEVICEID "/?api-version=2020-09-30"

#define DEVICETWIN_SUBSCRIPTION  "$iothub/twin/res/#"
#define DEVICETWIN_MESSAGE_GET   "$iothub/twin/GET/?$rid=%d"
#define DEVICETWIN_MESSAGE_PATCH "$iothub/twin/PATCH/properties/reported/?$rid=%d"

static unsigned int device_twin_request_id = 0;

void connect_callback(struct mosquitto* mosq, void* obj, int result)
{
    printf("Connect callback result: %s\r\n", mosquitto_strerror(result));
    if (result != MOSQ_ERR_SUCCESS)
    {
        printf("Connection error. Please check DeviceId, IoTHub name or if your SAS Token has expired.\r\n");
        return;
    }

    printf("\r\nSetting device twin reported properties....\r\n");

    char msg[] = "{\"temperature\": 32}";
    char mqtt_publish_topic[64];
    snprintf(mqtt_publish_topic, sizeof(mqtt_publish_topic), DEVICETWIN_MESSAGE_PATCH, device_twin_request_id++);

    int rc = mosquitto_publish(mosq, NULL, mqtt_publish_topic, sizeof(msg) - 1, msg, 1, true);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        printf("Error: %s\r\n", mosquitto_strerror(rc));
    }
}

void disconnect_callback(struct mosquitto* mosq, void* obj, int rc)
{
    printf("Disconnect callback result: %s\r\n", mosquitto_strerror(rc));
}

void message_callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message)
{
    printf("Device twin message '%.*s' for topic '%s'\r\n", message->payloadlen, (char*)message->payload, message->topic);

    const char patchTwinTopic[] = "$iothub/twin/res/204/?$rid=0";
    const char getTwinTopic[]   = "$iothub/twin/res/200/?$rid=1";

    if (strncmp(message->topic, patchTwinTopic, sizeof(patchTwinTopic) - 1) == 0)
    {
        // Process the reported property response and request the device twin
        printf("Setting device twin properties SUCCEEDED.\r\n\r\n");

        printf("Getting device twin properties....\r\n");

        char msg[] = "{}";
        char mqtt_publish_topic[64];
        snprintf(mqtt_publish_topic, sizeof(mqtt_publish_topic), DEVICETWIN_MESSAGE_GET, device_twin_request_id++);

        int rc = mosquitto_publish(mosq, NULL, mqtt_publish_topic, sizeof(msg) - 1, msg, 1, true);
        if (rc != MOSQ_ERR_SUCCESS)
        {
            printf("Error: %s\r\n", mosquitto_strerror(rc));
        }
    }
    else if (strncmp(message->topic, getTwinTopic, sizeof(getTwinTopic) - 1) == 0)
    {
        // Process the device twin response and stop the client
        printf("Getting device twin properties SUCCEEDED.\r\n\r\n");

        mosquitto_loop_stop(mosq, false);
        mosquitto_disconnect(mosq); // finished, exit program
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

    // create the mosquito object
    struct mosquitto* mosq = mosquitto_new(DEVICEID, false, NULL);

    // add callback functions
    mosquitto_connect_callback_set(mosq, connect_callback);
    mosquitto_disconnect_callback_set(mosq, disconnect_callback);
    mosquitto_message_callback_set(mosq, message_callback);

    // set mosquitto username & password
    mosquitto_username_pw_set(mosq, USERNAME, SAS_TOKEN);

    // specify the certificate
    printf("Using certificate: %s\r\n", CERTIFICATEFILE);
    rc = mosquitto_tls_set(mosq, CERTIFICATEFILE, NULL, NULL, NULL, NULL);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        printf("Error: Couldn't find file '%s'\r\n", CERTIFICATEFILE);
        return mosquitto_error(rc);
    }

    // connect
    printf("Connecting...\r\n");
    rc = mosquitto_connect(mosq, HOST, PORT, 10);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }
    printf("Connect returned OK\r\n");

    rc = mosquitto_subscribe(mosq, NULL, DEVICETWIN_SUBSCRIPTION, 0);
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
