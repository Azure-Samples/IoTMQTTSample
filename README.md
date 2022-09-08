---
page_type: sample
languages:
- c, python
products:
- azure-iot-hub
name: "IoT MQTT Samples"
description: "Using MQTT with Azure IoTHub without SDK"
urlFragment: "IoTMQTTSample"
---

# Using MQTT with Azure IoT Hub without an SDK

This set of samples will demonstrate how to connect and send messages to an Azure IoT Hub without using the Azure IoT SDKs.

Read the [Communicate with your IoT hub using the MQTT protocol](https://docs.microsoft.com/azure/iot-hub/iot-hub-mqtt-support) for detailed instructions on connecting your device to IoT Hub using MQTT without an SDK.

## Samples in this repo

This repository contains the following samples:
* [Mosquitto on Windows](src/Windows)
* [Mosquitto on Linux](src/Linux)
* [Mosquitto_pub CLI](src/Mosquitto_pub)
* [Python](src/Python)

## Trusted root certificate

Many of the samples depend on the [DigiCert Baltimore Root Certificate](IoTHubRootCA_Baltimore.pem) which is hosted in this repository.

## Prerequisites

To be able to test these samples will need:
1. An Azure IoT Hub
1. A registered device
1. The Eclipse Mosquitto library (for some samples)

### Creating an Azure IoT Hub

To create an Azure IoT Hub, [follow these steps](https://docs.microsoft.com/azure/iot-hub/iot-hub-create-through-portal).

### Registering a device

To register a new device and retrieve the connection information, [follow these steps](https://docs.microsoft.com/azure/iot-hub/iot-hub-create-through-portal#register-a-new-device-in-the-iot-hub).

## Mosquitto Prerequisites

For Mosquitto samples, you will need:
1. Install the Eclipse Mosquitto library
1. Generate SAS token
1. Complete the connection information

### Installing Eclipse Mosquitto

The Eclipse Mosquitto library and documentation are available at <https://mosquitto.org>.

> **NOTE:** If you use the mosquitto.org installer, only the files (header and lib) are needed for these samples, you can uncheck the service installation.

It is easier to use apt-get on Linux or vcpkg on Windows for installing Eclipse Mosquitto on your machine (see the corresponding readme in Linux and Windows directories).

### Generate SAS token

To get a valid IoT Hub SAS token, follow the instructions use the [generate-sas-token](https://docs.microsoft.com/cli/azure/iot/hub?view=azure-cli-latest#az-iot-hub-generate-sas-token) function from AZ CLI.

> **NOTE:** By default the **SAS Token is valid for 60 minutes**. Use the --du parameter to increases the duration if needed.

### Complete connection information

You will need to provide connection information and a [root certificate](IoTHubRootCA_Baltimore.pem).

In the code, complete the connection information:

```c
#define IOTHUBNAME      <MyIoTHub>
#define DEVICEID        <DeviceId>
#define CERTIFICATEFILE "<pathTo>\\IoTHubRootCA_Baltimore.pem"
#define PWD             "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[DeviceId]&sig=[tokengeneratedforyourdevice]"
```

## Contributing

For details on contributing to this repository, see the [contributing](CONTRIBUTING.md) guide.

## Reporting Security Vulnerabilities

If you believe you have found a security vulnerability in any Microsoft-owned repository that meets Microsoft's definition of a security vulnerability, please report it to the [Microsoft Security Response Center](SECURITY.md).
