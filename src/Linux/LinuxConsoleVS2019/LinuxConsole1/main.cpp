#include <cstdio>
#include "mosquitto.h"
#include "mosquitto.h"

//CONNECTION information to complete
#define IOTHUBNAME "<your IoT Hub name>"
#define DEVICEID "<your device Id>"
#define PWD "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[nameofyourdevice]&sig=[tokengeneratedforyourdevice]"
#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"

//Certificate
// Server certs available here for download: https://raw.githubusercontent.com/Azure/azure-iot-sdk-c/master/certs/certs.c 


//PWD
// generated via Azure CLI, Device explorer or VSCode Azure IoT extension (Generate SAS Token for device)
// az iot hub generate-sas-token -d EM_MXC3166 -n EricmittHub

//Username
// username format for MQTT connection to Hub: $hub_hostname/$device_id/?api-version=2018-06-30" 
#define USERNAME "EricmittHUB.azure-devices.net/EM_MXC3166/?api-version=2018-06-30"

//computed Host Username and Topic
// not with g++
//#define USERNAME  IOTHUBNAME/".azure-devices.net/"DEVICEID"/?api-version=2018-06-30"
#define PORT 8883
#define HOST "EricmittHUB.azure-devices.net"
#define TOPIC "devices/EM_MXC3166/messages/events/"


//Callback functions
void connect_callback(struct mosquitto* mosq, void* obj, int result)
{
	char msg[256];
	//sprintf_s(msg, sizeof(msg), "Connect Callback returned : %d \r\n", result);
	sprintf(msg,"Connect Callback returned : %d \r\n", result);
	printf(msg);

	if (result == MOSQ_ERR_CONN_REFUSED)
		printf("Connection Refused, please check your SAS Token, expired ?\r\n");

}


void publish_callback(struct mosquitto* mosq, void* userdata, int mid)
{
	mosquitto_disconnect(mosq);
	printf("Publish OK, now disconnecting the client");
}


int main()
{
	printf("Use MQTT to send message to %s\n", HOST);

	int rc;

	//init the mosquitto lib
	mosquitto_lib_init();

	// create a mosquito 'object'
	struct mosquitto* mosq;
	mosq = mosquitto_new(DEVICEID, false, NULL);

	// add callback fuctions
	mosquitto_connect_callback_set(mosq, connect_callback);
	mosquitto_publish_callback_set(mosq, publish_callback);

	// precise mosquitto username, password and options
	mosquitto_username_pw_set(mosq, USERNAME, PWD);

	//specify the certificate to use
	mosquitto_tls_set(mosq, CERTIFICATEFILE, NULL, NULL, NULL, NULL);

	//specify the mqtt version to use
	int* option = new int();
	*option = MQTT_PROTOCOL_V311;
	rc = mosquitto_opts_set(mosq, MOSQ_OPT_PROTOCOL_VERSION, option);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		printf("error opts_set protocol version");
	}

	// connect
	rc = mosquitto_connect(mosq, HOST, PORT, 10);
	if (rc) {
		printf("Error: %s\n", mosquitto_strerror(rc));
		mosquitto_lib_cleanup();
		return rc;
	}


	int msgId = 42;
	char msg[] = "Bonjour MQTT";

	//once connected we can publish (send) a message
	rc = mosquitto_publish(mosq, &msgId, TOPIC, sizeof(msg) - 1, msg, 1, true);
	if (rc) {
		printf("Error: %s\n", mosquitto_strerror(rc));
		mosquitto_lib_cleanup();
		return rc;
	}

	// according to the mosquitto doc, a call to loop is needed when dealing with network operation
	// see https://github.com/eclipse/mosquitto/blob/master/lib/mosquitto.h
	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();
}
