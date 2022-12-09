// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <cstdio>
#include <fstream>

#include "mosquitto.h"

#define IOTHUBNAME      "{iothub_name}"
#define DEVICEID        "{device_id}"
#define SAS_TOKEN       "{sas_token}"
#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"

// computed Host, Username and Topic
#define HOST     IOTHUBNAME ".azure-devices.net"
#define PORT     8883
#define USERNAME HOST "/" DEVICEID "/?api-version=2018-06-30"
#define TOPIC    "devices/" DEVICEID "/messages/events/"

// callback functions
void connect_callback(struct mosquitto* client, void* obj, int result)
{
    printf("Connect callback returned result: %s\r\n", mosquitto_strerror(result));

    if (result == MOSQ_ERR_CONN_REFUSED)
    {
        printf("Connection refused. Please check DeviceId, IoTHub name or if your SAS Token has expired.\r\n");
    }
}

void publish_callback(struct mosquitto* client, void* userdata, int mid)
{
    printf("Publish OK. Now disconnecting the client.\r\n");

    mosquitto_disconnect(client);
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

int main()
{
    printf("Using MQTT to send message to %s.\r\n", HOST);

    // init the mosquitto lib
    mosquitto_lib_init();

    // create the mosquito object
    struct mosquitto* client = mosquitto_new(DEVICEID, false, NULL);

    // add callback functions
    mosquitto_connect_callback_set(client, connect_callback);
    mosquitto_publish_callback_set(client, publish_callback);

    // set mosquitto username & password
    mosquitto_username_pw_set(client, USERNAME, SAS_TOKEN);

    // specify the certificate
    std::ifstream infile(CERTIFICATEFILE);
    bool certExists = infile.good();
    infile.close();
    if (!certExists)
    {
        printf("Warning: Could not find file '%s'! The mosquitto loop may fail.\r\n", CERTIFICATEFILE);
    }

    printf("Using certificate: %s\r\n", CERTIFICATEFILE);
    mosquitto_tls_set(client, CERTIFICATEFILE, NULL, NULL, NULL, NULL);

    // connect
    printf("Connecting...\r\n");
    int rc = mosquitto_connect(client, HOST, PORT, 10);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }

    printf("Connect returned OK\r\n");

    int msgId  = 42;
    char msg[] = "Bonjour MQTT from Mosquitto";

    // once connected, we can publish a Telemetry message
    printf("Publishing....\r\n");
    rc = mosquitto_publish(client, &msgId, TOPIC, sizeof(msg) - 1, msg, 1, true);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        return mosquitto_error(rc);
    }

    printf("Publish returned OK\r\n");

    // according to the mosquitto doc, a call to loop is needed when dealing with network operation
    // see https://github.com/eclipse/mosquitto/blob/master/lib/mosquitto.h
    printf("Entering Mosquitto Loop...\r\n");
    mosquitto_loop_forever(client, -1, 1);

    mosquitto_lib_cleanup();
}
