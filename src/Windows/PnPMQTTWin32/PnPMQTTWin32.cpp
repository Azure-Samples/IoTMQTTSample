// Access IoTHub via MQTTT Protocol with mosquitto library (no IoT SDK used)
// Use PnP Convention 
//

#include <cstdio>
#include <fstream>
#include <thread>
#include <future>

#include "mosquitto.h"

#include <windows.h>

// CONNECTION information to complete
#define IOTHUBNAME "[yourIoTHub]"
#define DEVICEID "[yourDevice]"
#define PWD "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[nameofyourdevice]&sig=[tokengeneratedforyourdevice]"
// You need to generate a SAS Token valid for couple of hours, 
// use IoT  Hub portal or IoTExplore to generate one

#define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"

// computed Host Username and Topic
// #define USERNAME IOTHUBNAME ".azure-devices.net/" DEVICEID "/?api-version=2019-07-01-preview"
// Updated API version for Pnp preview refresh summer 2020
#define USERNAME IOTHUBNAME ".azure-devices.net/" DEVICEID "/?api-version=2020-05-31-preview&model-id=dtmi:com:example:Thermostat;1"
#define PORT 8883
#define HOST IOTHUBNAME ".azure-devices.net"

// MQTT Topics
#define	DEVICE_TWIN_DESIRED_PROP_RES_TOPIC "$iothub/twin/PATCH/properties/desired/#"
#define DEVICE_TWIN_RES_TOPIC "$iothub/twin/res/#"
#define DIRECT_METHOD_TOPIC "$iothub/methods/POST/#"
#define TOPIC "devices/" DEVICEID "/messages/events/"
#define DEVICETWIN_MESSAGE_PATCH "$iothub/twin/PATCH/properties/reported/?$rid=patch_temp"


static struct mosquitto* mosq;

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

static void SendMaxTemperatureSinceReboot()
{
	char msg[] = "{\"maxTempSinceLastReboot\": 42.500}";
	int msgId = rand();
	int rc = mosquitto_publish(mosq, &msgId, DEVICETWIN_MESSAGE_PATCH, sizeof(msg) - 1, msg, 1, true);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		printf("Error: %s\r\n", mosquitto_strerror(rc));
	}
}

static void SendTargetTemperatureReport(double desiredTemp, int responseStatus, int version, const char* description)
{
	char msg[] = "{\"targetTemperature\":{\"value\":38,\"ac\":200,\"av\":23,\"ad\":\"success\"}}";
	int msgId = rand();
	int rc = mosquitto_publish(mosq, &msgId, DEVICETWIN_MESSAGE_PATCH, sizeof(msg) - 1, msg, 1, true);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		printf("Error: %s\r\n", mosquitto_strerror(rc));
	}
}

void Thermostat_SendCurrentTemperature()
{
	char msg[] = "{\"temperature\":25}";
	int msgId = rand();
	int rc = mosquitto_publish(mosq, &msgId, TOPIC, sizeof(msg) - 1, msg, 1, true);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		printf("Error: %s\r\n", mosquitto_strerror(rc));
	}
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
		SendMaxTemperatureSinceReboot();
		SendTargetTemperatureReport(-273,200,25,"targetTemp");
	}
}

void publish_callback(struct mosquitto* mosq, void* userdata, int msgId)
{
	printf("Publish OK. mid=%d userData: %s\r\n", msgId, (char*)userdata);
	Sleep(2000);
	Thermostat_SendCurrentTemperature();
}

void message_callback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* message)
{
	printf("Message received: %s payload: %s \r\n", message->topic, (char*)message->payload);
	
	if (strncmp(message->topic, "$iothub/methods/POST/getMaxMinReport/?$rid=1",37) == 0)
	{
		char* pch;
		char* context;
		int msgId = 0;
		pch = strtok_s((char*)message->topic, "=",&context);
		while (pch != NULL)
		{
			pch = strtok_s(NULL, "=", &context);
			if (pch != NULL) {
				char * pEnd;
				msgId = strtol(pch,&pEnd,16 );
			}
		}
		char topic[64];
		sprintf_s(topic, "$iothub/methods/res/200/?$rid=%d", msgId);
		char msg[] = "{}";
		printf("rid = %d\n", msgId);
		int rc = mosquitto_publish(mosq, &msgId, topic, sizeof(msg) - 1, msg, 1, true);
		if (rc != MOSQ_ERR_SUCCESS)
		{
			printf("Error: %s\r\n", mosquitto_strerror(rc));
		}
		delete pch;
	}
}

int main()
{
	int rc;
	printf("Using MQTT to send message to %s.\r\n", HOST);

	// init the mosquitto lib
	mosquitto_lib_init();

	// create the mosquito object
	 mosq = mosquitto_new(DEVICEID, false, NULL);

	// add callback functions
	mosquitto_connect_callback_set(mosq, connect_callback);
	mosquitto_publish_callback_set(mosq, publish_callback);
	mosquitto_message_callback_set(mosq, message_callback);

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

	
	rc = mosquitto_subscribe(mosq, NULL, DEVICE_TWIN_RES_TOPIC, 0);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		return mosquitto_error(rc);
	}
	rc = mosquitto_subscribe(mosq, NULL, DIRECT_METHOD_TOPIC, 0);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		return mosquitto_error(rc);
	}
	rc = mosquitto_subscribe(mosq, NULL, DEVICE_TWIN_DESIRED_PROP_RES_TOPIC, 0);
	if (rc != MOSQ_ERR_SUCCESS)
	{
		return mosquitto_error(rc);
	}

	printf("Entering Mosquitto Loop...\r\n");
	mosquitto_loop_forever(mosq, -1, 1);
	
	mosquitto_lib_cleanup();
}
