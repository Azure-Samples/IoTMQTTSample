# Using MQTT PnP with Azure IoTHub without the IoT SDK on Windows

## PnP Convention

This samples uses the Mosquitto (MQTT) library to send IoT PnP messages to an Azure IoT Hub from Windows.

This sample follow the PnP convention "Preview Refresh" and use [DTDL v2](https://github.com/Azure/opendigitaltwins-dtdl/blob/master/DTDL/v2/dtdlv2.md).

This program implements the [Thermostat model](https://github.com/Azure/opendigitaltwins-dtdl/blob/master/DTDL/v2/samples/Thermostat.json)

- Provide a PnP model-id at startup.
- Initialize a set of "read only" properties.
- Send telemetry message.
- Receive device twin property changes.
- Handle Pnp direct method call.

All the topics used for properties and method, are described in this [Azure IoT MQTT article](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#receiving-cloud-to-device-messages)

## Validate your code

We suggest to use [Azure IoT Explorer](https://github.com/Azure/azure-iot-explorer/releases/tag/v0.11.1), this version allow you to explorer digital property for the PnP device and components.

Note, that you can create or delete device, get SAS Key, send command, receive telemetry etc with IoTExplorer.
