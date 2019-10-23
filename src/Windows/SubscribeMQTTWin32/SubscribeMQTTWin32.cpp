//Subscribe to an IoT Hub events
//


#include <stdlib.h>
#include <stdio.h>
#include "mosquitto.h"

// Azure IoT Hub CONNECTION information to complete
#define IOTHUBNAME "<your IoT Hub name>"
#define DEVICEID "your device ID"
#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"
#define PWD "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[nameofyourdevice]&sig=[tokengeneratedforyourdevice]"

//Certificate
// Server certs available here for download: https://raw.githubusercontent.com/Azure/azure-iot-sdk-c/master/certs/certs.c

//PWD
// generated via Azure CLI, Device explorer or VSCode Azure IoT extension (Generate SAS Token for device)
// az iot hub generate-sas-token -d EM_AZSphere -n <your IoT Hub name>

//Username
// username format for MQTT connection to Hub: $hub_hostname/$device_id/?api-version=2018-06-30"
//#define USERNAME "EricmittHUB.azure-devices.net/EM_AZSphere/?api-version=2018-06-30"

// Computed Host Username and Topic
#define USERNAME IOTHUBNAME ".azure-devices.net/" DEVICEID "/?api-version=2018-06-30"
#define PORT 8883
#define HOST IOTHUBNAME ".azure-devices.net"
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
	struct mosquitto*  mosq = mosquitto_new(DEVICEID, false, NULL);

	// add callback fuctions
	mosquitto_connect_callback_set(mosq, connect_callback);
	mosquitto_message_callback_set(mosq, message_callback);
	mosquitto_disconnect_callback_set(mosq, disconnect_callback);

	// specify mosquitto username, password and options
	mosquitto_username_pw_set(mosq, USERNAME, PWD);

	// specify the certificate to use
	mosquitto_tls_set(mosq, CERTIFICATEFILE, NULL, NULL, NULL, NULL);

	// specify the mqtt version to use
	int option = MQTT_PROTOCOL_V311;
	rc = mosquitto_opts_set(mosq, MOSQ_OPT_PROTOCOL_VERSION, &option);
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
