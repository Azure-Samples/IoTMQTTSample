#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mosquitto.h"

// Azure IoT Hub CONNECTION information to complete
#define IOTHUBNAME "<your IoT Hub name>"
#define DEVICEID "<Your device ID>"
#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"
#define PWD "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[nameofyourdevice]&sig=[tokengeneratedforyourdevice]"

//Certificate
// Server certs available here for download: https://raw.githubusercontent.com/Azure/azure-iot-sdk-c/master/certs/certs.c

//PWD
// generated via Azure CLI, Device explorer or VSCode Azure IoT extension (Generate SAS Token for device)
// az iot hub generate-sas-token -d EM_AZSphere -n EricmittHub --duration 2600000

//Username
// username format for MQTT connection to Hub: $hub_hostname/$device_id/?api-version=2018-06-30"
//#define USERNAME "EricmittHUB.azure-devices.net/EM_AZSphere/?api-version=2018-06-30"

// Computed Host Username
#define USERNAME IOTHUBNAME ".azure-devices.net/" DEVICEID "/?api-version=2018-06-30"
#define PORT 8883
#define HOST IOTHUBNAME ".azure-devices.net"

// MQTT 
#define DEVICEMESSAGE "devices/" DEVICEID "/messages/#"
#define DEVICETWIN_SUBSCRIPTION "$iothub/twin/res/#"
#define DEVICETWIN_MESSAGE_GET "$iothub/twin/GET/?$rid=123"
#define DEVICETWIN_MESSAGE_PATCH "$iothub/twin/PATCH/properties/reported/?$rid=patch_temp"

void connect_callback(struct mosquitto* mosq, void* obj, int result)
{
	printf("Connect callback returned result: %s\r\n", mosquitto_strerror(result));
	if (result == MOSQ_ERR_CONN_REFUSED)
	{
		printf("Connection refused. Please check your SAS Token, expired ?\r\n");
	}
	else
	{
		printf("\r\nSetting device twin reported properties....\r\n");
		int msgId = rand();
		char msg[] = "{\"temperature\": 32}";
		int rc = mosquitto_publish(mosq, &msgId, DEVICETWIN_MESSAGE_PATCH, sizeof(msg) - 1, msg, 1, true);
		if (rc != MOSQ_ERR_SUCCESS)
		{
			printf("Error: %s\r\n", mosquitto_strerror(rc));
		}
	}
}

void disconnect_callback(struct mosquitto* mosq, void* obj, int rc)
{
	printf("Disconnect callback %d : %s\r\n", rc, mosquitto_strerror(rc));
}

void message_callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message)
{
	printf("Device twin message '%.*s' for topic '%s'\r\n", message->payloadlen, (char* )message->payload, message->topic);
	
	const char getTwinTopic[] = "$iothub/twin/res/200/?$rid=123";
	const char patchTwinTopic[] = "$iothub/twin/res/204/?$rid=patch_temp";
	if (strncmp(message->topic, patchTwinTopic, sizeof(patchTwinTopic)-1) == 0)
	{
		printf("Setting device twin properties SUCCEEDED.\r\n\r\n");
		printf("Getting device twin properties....\r\n");
		int msgId = rand();
		char msg[] = "";
		int rc = mosquitto_publish(mosq, &msgId, DEVICETWIN_MESSAGE_GET, sizeof(msg) - 1, msg, 1, true);
		if (rc != MOSQ_ERR_SUCCESS)
		{
			printf("Error: %s\r\n", mosquitto_strerror(rc));
		}
	}
	else if (strncmp(message->topic, getTwinTopic, sizeof(getTwinTopic) - 1) == 0)
	{
		printf("Getting device twin properties SUCCEEDED.\r\n\r\n");
		mosquitto_loop_stop(mosq, false);
		mosquitto_disconnect(mosq);  // finished, exit program
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
