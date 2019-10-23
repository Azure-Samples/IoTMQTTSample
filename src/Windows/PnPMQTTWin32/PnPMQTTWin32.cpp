// Access IoTHub via MQTTT Protocol with mosquitto library (no IoT SDK used)
//

#include <cstdio>
#include <fstream>
#include "mosquitto.h"

#include <windows.h>

// CONNECTION information to complete
#define IOTHUBNAME "<your IoT Hub name>"
#define DEVICEID "<your device Id>"
#define PWD "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[nameofyourdevice]&sig=[tokengeneratedforyourdevice]"

#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"

// computed Host Username and Topic
#define USERNAME IOTHUBNAME ".azure-devices.net/" DEVICEID "/?api-version=2019-07-01-preview"
#define PORT 8883
#define HOST IOTHUBNAME ".azure-devices.net"
#define DEVICE_CAPABILITIES_MESSAGE "devices/" DEVICEID "/messages/events/%24.ifid=urn%3aazureiot%3aModelDiscovery%3aModelInformation%3a1&%24.ifname=urn_azureiot_ModelDiscovery_ModelInformation&%24.schema=modelInformation&%24.ct=application%2fjson"
#define DEVICETWIN_PATCH_MESSAGE "$iothub/twin/PATCH/properties/reported/?$rid=patch_temp"
#define DEVICE_TELEMETRY_MESSAGE "devices/" DEVICEID "/messages/events/%24.ifid=urn%3amxchip%3abuilt_in_sensors%3a1&%24.ifname=sensor&%24.schema=Temperature&%24.ct=application%2fjson"

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

void publish_next_packet(struct mosquitto* mosq, int msgId)
{
	const char* msg = NULL;
	const char* topic = NULL;
	switch (msgId)
	{
		case 0:  // Let Azure IoT know about this device capabilities, by publishing a PnP Device Model Telemetry message
			printf("\r\nPublishing device capabilities....\r\n");
			topic = DEVICE_CAPABILITIES_MESSAGE;
			msg = "{"
					"\"modelInformation\":"
					"{"
						"\"capabilityModelId\":\"urn:mxchip:sample_device:1\","
						"\"interfaces\":"
						"{"
							"\"urn_azureiot_ModelDiscovery_ModelInformation\":\"urn:azureiot:ModelDiscovery:ModelInformation:1\","
							"\"urn_azureiot_Client_SDKInformation\":\"urn:azureiot:Client:SDKInformation:1\","
							"\"deviceinfo\":\"urn:azureiot:DeviceManagement:DeviceInformation:1\","
							"\"sensor\":\"urn:mxchip:built_in_sensors:1\""
						"}"
					"}"
				"}";
			break;

		case 1: 
			printf("\r\nPublishing device twin properties....\r\n");
			topic = DEVICETWIN_PATCH_MESSAGE;
			msg = "{"
					"\"$iotin:urn_azureiot_Client_SDKInformation\":"
					"{"
						"\"language\":{\"value\":\"C\"},"
						"\"version\":{\"value\":\"0.0.1\"},"
						"\"vendor\":{\"value\":\"umthengisi\"}"
					"},"
					"\"$iotin:deviceinfo\":"
					"{"
						"\"manufacturer\":{\"model\":\"imodeli\"},"
						"\"swVersion\":{\"value\":\"0.0.1\"}"
					"}"
				"}";
			break;

		case 2:
			printf("\r\nPublishing device telemetry....\r\n");
			topic = DEVICE_TELEMETRY_MESSAGE;
			msg = "{ \"Temperature\": 32.000 }";
			break;

		default:
			printf("\r\nDone....\r\n");
			mosquitto_loop_stop(mosq, false);
			mosquitto_disconnect(mosq);
			return;
	}

	msgId++;
	int rc = mosquitto_publish(mosq, &msgId, topic, (int)strlen(msg), msg, 1, false);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		mosquitto_error(rc);
	}
	printf("Publish returned OK\r\n");
}

// Callback functions
void connect_callback(struct mosquitto* mosq, void* obj, int result)
{
	printf("Connect callback returned result: %s\r\n", mosquitto_strerror(result));
	if (result == MOSQ_ERR_CONN_REFUSED)
	{
		printf("Connection refused. Please check DeviceId, IoTHub name or if your SAS Token has expired.\r\n");
	}
	else
	{
		publish_next_packet(mosq, 0);
	}
}

void publish_callback(struct mosquitto* mosq, void* userdata, int msgId)
{
	printf("Publish OK. mid=%d\r\n", msgId);
	publish_next_packet(mosq, msgId);
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


	// according to the mosquitto doc, a call to loop is needed when dealing with network operation
	// see https://github.com/eclipse/mosquitto/blob/master/lib/mosquitto.h
	printf("Entering Mosquitto Loop...\r\n");
	mosquitto_loop_forever(mosq, -1, 1);
	
	mosquitto_lib_cleanup();
}
