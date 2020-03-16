---
page_type: sample
languages:
- c
products:
- azure-iot-hub
name: "IoT MQTT Samples"
description: "Using MQTT with Azure IoTHub without SDK"
urlFragment: "IoTMQTTSample"
---

# Using MQTT with Azure IoTHub without SDK

In this set of samples, we will show how to connect and send messages to an Azure IoT Hub without using the Azure IoT SDK. This approach is useful when you use constrained device who don’t have enough memory to support the Azure IoT C SDK. These sample code uses the Eclipse Mosquitto library to send messages directly to the IoTHub via the MQTT Protocol.
This repository contains sample for [Windows](https://github.com/Azure-Samples/IoTMQTTSample/tree/master/src/Windows) and [Linux](https://github.com/Azure-Samples/IoTMQTTSample/tree/master/src/Linux) and a couple of [sample command](https://github.com/Azure-Samples/IoTMQTTSample/tree/master/src/Mosquitto_pub) to use with mosquitto_pub tool (tool included in the Mosquitto library package).

Find also a sample using [MicroPython](https://github.com/Azure-Samples/IoTMQTTSample/tree/master/src/MicroPython) and umqtt library, this sample run on [ESP32](https://github.com/Azure-Samples/ESP-Samples) and demonstrate how to send telemetry message and receive messages from cloud.

## Contents

Below is the file contents of this repository. 

| File/folder                  | Description                                |
|------------------------------|--------------------------------------------|
| `src`                        | Sample source code.
| `.gitignore`                 | Defines what to ignore at commit time.     |
| `README.md`                  | This README file.                          |
| `LICENSE`                    | The license for the sample.                |
| `SECURITY.md`                | Security information                       |
| `CODE_OF_CONDUCT.md`         | Microsoft code of conduct for contribution |
| `IoTHubRootCA_Baltimore.pem` | certificate used in these samples          |

## Prerequisites

To be able to test these samples you need an Azure IoTHub and installing Eclipse Mosquitto Library.


For MicroPython, follow the steps in this [readme file](https://github.com/Azure-Samples/IoTMQTTSample/tree/master/src/MicroPython/readme.md) to flash MicroPython on your ESP32 install umqtt library and use it for connecting Azure IoT Hub.

### Creating an Azure IoT Hub

This sample needs an IoT hub and a registered device. If you don't already have an Azure IoT Hub, you can create one following these instructions:

1. Install the [Azure CLI](https://docs.microsoft.com/en-us/cli/azure/install-azure-cli?view=azure-cli-latest) (or use the [Azure Cloud Shell](https://shell.azure.com/)) and use it to [create an Azure IoT Hub](https://docs.microsoft.com/en-us/cli/azure/iot/hub?view=azure-cli-latest#az-iot-hub-create).

    ```bash
    az iot hub create --resource-group <your resource group> --name <your IoT Hub name>
    ```

    > Note that this operation may take a few minutes.

1. Add the IoT Extension to the Azure CLI, and then [register a device identity](https://docs.microsoft.com/en-us/cli/azure/ext/azure-cli-iot-ext/iot/hub/device-identity?view=azure-cli-latest#ext-azure-cli-iot-ext-az-iot-hub-device-identity-create)

    ```bash
    az extension add --name azure-cli-iot-ext
    az iot hub device-identity create --hub-name <your IoT Hub name> --device-id <your device id>
    ```

1. [Retrieve your Device Connection String](https://docs.microsoft.com/en-us/cli/azure/ext/azure-cli-iot-ext/iot/hub/device-identity?view=azure-cli-latest#ext-azure-cli-iot-ext-az-iot-hub-device-identity-show-connection-string) using the Azure CLI

    ```bash
    az iot hub device-identity show-connection-string --device-id <your device id> --hub-name <your IoT Hub name>
    ```

    It should be in the format:

    `HostName=<your IoT Hub name>.azure-devices.net;DeviceId=<your device id>;SharedAccessKey=<some value>`

## Installing Eclipse Mosquitto library

The Eclipse Mosquitto library and documentation are available at <https://mosquitto.org/>.

> Note: If you use the mosquitto.org installer, only the files (header and lib) are needed for these samples, you can uncheck the service installation.

It is easier to use apt-get on Linux or vcpkg on Windows for installing Eclipse Mosquitto on your machine (see the corresponding readme in Linux and Windows directories).

## Setup

Once Eclipse Mosquitto is installed correctly on your machine, you will need to provide connection information and a security certificate.

The certificate to use with these samples is located in this repo (IoTHubRootCA_Baltimore.pem)
> Server certificates are available [here](https://raw.githubusercontent.com/Azure/azure-iot-sdk-c/master/certs/certs.c) for download. 

To get a valid IoT hub SAS Token, run this command in the Azure IoT Command line (see above for installing the Azure IoT extension to the Azure Command line)

`az iot hub generate-sas-token -d <DeviceId> -n <MyIoTHub>`

> Note: That by default the **SAS Token is only valid for 60 min**. You can use the --du parameter to get the duration you need. You can also use Device Explorer to generate a SAS Token or VSCode Azure Iot Extension.

> For more information on how to create SAS Token see [here](https://docs.microsoft.com/en-us/azure/iot-pnp/howto-generate-sas-token)

Then in the code, complete the connection information:

```c
#define IOTHUBNAME <MyIoTHub>
#define DEVICEID <DeviceId>
#define CERTIFICATEFILE "<pathTo>\\IoTHubRootCA_Baltimore.pem"
#define PWD "SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[DeviceId]&sig=[tokengeneratedforyourdevice]"
```

> Note: The SAS token should be used as password see this [article](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#using-the-mqtt-protocol-directly-as-a-device) for more details on  password, username and configuration needed for using MQTT protocol.

## Key concepts

IoT Hub under the cover is a MQTT server (but not a full MQTT broker as explained [here](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#using-the-mqtt-protocol-directly-as-a-device)), so by using the MQTT protocol, we can communicate (send and receive messages) with the IoT Hub without using the Azure IoT SDK.

In order to establish a TLS connection, you may need to download and reference the DigiCert Baltimore Root Certificate. This certificate is the one that Azure uses to secure the connection. You can find this certificate in the [Azure-iot-sdk-c](https://github.com/Azure/azure-iot-sdk-c/blob/master/certs/certs.c) repository. More information about these certificates can be found on [Digicert's website](https://www.digicert.com/digicert-root-certificates.htm).
The client use the certificate to check that the IoT Hub it's connecting to is indeed legit, by providing the public key of the certificate used by Microsoft to secure IoT Hub connections. 


You need to secure the communication and provide a certificate to be accepted by the MQTT broker under the Azure IoTHub.

The information/credential needed by the MQTT broker, are the classic pair: user/password, in the case of Azure IoT Hub the password should be the SAS Token, and the username should have this form:

`IOTHUBNAME.azure-devices.net/DEVICEID/?api-version=2018-06-30`

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit <https://cla.opensource.microsoft.com>.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
