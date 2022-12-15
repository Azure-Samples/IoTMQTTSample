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

#define TOPIC "devices/" DEVICEID "/messages/events/"

void connect_callback(struct mosquitto* mosq, void* obj, int result)
{
    printf("Connect callback returned result: %s\r\n", mosquitto_strerror(result));
    if (result == MOSQ_ERR_CONN_REFUSED)
    {
        printf("Connection refused. Please check DeviceId, IoTHub name or if your SAS Token has expired.\r\n");
    }
}

void disconnect_callback(struct mosquitto* mosq, void* obj, int rc)
{
    printf("Disconnect callback %d : %s\r\n", rc, mosquitto_strerror(rc));
}

void publish_callback(struct mosquitto* mosq, void* userdata, int mid)
{
    printf("Publish OK. Now disconnecting the client.\r\n");

    mosquitto_disconnect(mosq);
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
    mosquitto_publish_callback_set(mosq, publish_callback);

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

    int msgId  = 42;
    char msg[] = "Bonjour MQTT from Mosquitto";

    // once connected, we can publish a Telemetry message
    printf("Publishing....\r\n");
    rc = mosquitto_publish(mosq, &msgId, TOPIC, sizeof(msg) - 1, msg, 1, true);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }
    printf("Publish returned OK\r\n");

    rc = mosquitto_loop_forever(mosq, -1, 1);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }

    mosquitto_lib_cleanup();
    return 0;
}
