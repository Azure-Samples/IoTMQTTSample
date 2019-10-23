// Access IoTHub via MQTTT Protocol with mosquitto library (no IoT SDK used)
//

#include <cstdio>
#include <fstream>
#include "mosquitto.h"

// CONNECTION information to complete
#define IOTHUBNAME "<your IoT Hub name>"
#define DEVICEID "<your device Id>"
#define PWD "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[nameofyourdevice]&sig=[tokengeneratedforyourdevice]"


#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"

// computed Host Username and Topic
#define USERNAME IOTHUBNAME ".azure-devices.net/" DEVICEID "/?api-version=2018-06-30"
#define PORT 8883
#define HOST IOTHUBNAME ".azure-devices.net"
#define TOPIC "devices/" DEVICEID "/messages/events/"

// Note
// Certificate
//  Server certs available here for download: https://raw.githubusercontent.com/Azure/azure-iot-sdk-c/master/certs/certs.c
// 
// PWD
//  Generated via Azure CLI, Device explorer or VSCode Azure IoT extension (Generate SAS Token for device)
//  az iot hub generate-sas-token -d EM_MXC3166 -n EricmittHub
// 
// Username
//  Username format for MQTT connection to Hub: $hub_hostname/$device_id/?api-version=2018-06-30"

// Callback functions
void connect_callback(struct mosquitto* mosq, void* obj, int result)
{
	printf("Connect callback returned result: %s\r\n", mosquitto_strerror(result));

	if (result == MOSQ_ERR_CONN_REFUSED)
		printf("Connection refused. Please check DeviceId, IoTHub name or if your SAS Token has expired.\r\n");
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

int main()
{
	int rc;
	printf("Using MQTT to send message to %s.\r\n", HOST);

	// init the mosquitto lib
	mosquitto_lib_init();

	// create the mosquito object
	struct mosquitto* mosq = mosquitto_new(DEVICEID, false, NULL);

	// add callback functions
	mosquitto_connect_callback_set(mosq, connect_callback);
	mosquitto_publish_callback_set(mosq, publish_callback);

	// set mosquitto username, password and options
	mosquitto_username_pw_set(mosq, USERNAME, PWD);

	// specify the certificate to use
	std::ifstream infile(CERTIFICATEFILE);
	bool certExists = infile.good();
	infile.close();
	if (!certExists)
	{
		printf("Warning: Could not find file '%s'! The mosquitto loop may fail.\r\n", CERTIFICATEFILE);
	}

	printf("Using certificate: %s\r\n", CERTIFICATEFILE);
	mosquitto_tls_set(mosq, CERTIFICATEFILE, NULL, NULL, NULL, NULL);

	// specify the mqtt version to use
	int* option = new int(MQTT_PROTOCOL_V311);
	rc = mosquitto_opts_set(mosq, MOSQ_OPT_PROTOCOL_VERSION, option);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		return mosquitto_error(rc, "Error: opts_set protocol version");
	}
	else
	{
		printf("Setting up options OK\r\n");
	}

	// connect
	printf("Connecting...\r\n");
	rc = mosquitto_connect(mosq, HOST, PORT, 10);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		return mosquitto_error(rc);
	}

	printf("Connect returned OK\r\n");

	int msgId = 42;
	char msg[] = "Bonjour MQTT from WSL";

	// once connected, we can publish (send) a Telemetry message
	printf("Publishing....\r\n");
	rc = mosquitto_publish(mosq, &msgId, TOPIC, sizeof(msg) - 1, msg, 1, true);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		return mosquitto_error(rc);
	}

	printf("Publish returned OK\r\n");

	// according to the mosquitto doc, a call to loop is needed when dealing with network operation
	// see https://github.com/eclipse/mosquitto/blob/master/lib/mosquitto.h
	printf("Entering Mosquitto Loop...\r\n");
	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();
}
