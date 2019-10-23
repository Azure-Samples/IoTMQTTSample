# Using MQTT with Azure IoTHub without SDK on Windows

These samples uses the Mosquitto (MQTT) library to send a message and subscribe to events to an Azure IoT Hub from Windows.

There are three samples in this folder:

- TelemetryMQTTWin32: contains code to send a telemetry message to an Azure IoT Hub to build and run on a Windows machine.
- SubscribeMQTTWin32: contains code to subscribe to events of a given IoT Hub on a Windows machine.
- DeviceTwinMQTTWin32: contains code to query and subscribe to the device twin events of a device in the Azure IoT Hub on a Windows machine.
- PnPMQTTWin32: contains code to send a telemetry message with PnP Device capabilities to an Azure IoT Hub to build and run on a Windows machine. 

## Prerequisite

On Windows with Visual Studio, use [vcpkg](https://github.com/Microsoft/vcpkg) package for installing Mosquitto.
Clone the vcpkg repository and build the tool:

```cmd
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

PS> .\bootstrap-vcpkg.bat
```

Then, to enable user-wide [integration](docs/users/integration.md), run (note requires admin on first use)

```cmd
PS> .\vcpkg integrate install
```

Once the vcpkg package is cloned on your machine, run the following commands:

`.\vcpkg install mosquitto:x64-windows`

> Note: Please check carefully if you are installing x64 or x86 version of the library. Your project should target the same platform. Additional tools from mosquitto, like mosquitto_pub, are installed by vcpkg in this folder: [vcpkg_root_dir]\buildtrees\mosquitto\x64-windows-rel\client

## Setup

Clone the current [github repository]( https://github.com/Azure-Samples/IoTMQTTSample) on your machine.

Open the solution file MQTTWin32.sln in Visual Studio 2019 (you need to install "Desktop Development with C++" workload)

## Running the code

Build and run the code (Crtl+F5 or just F5 to debug)

## Validate your code

### When sending message

Open an Azure CLI and execute:

`az iot hub monitor-events --hub-name <your IoT Hub name> --output table`

You should see something like:

![Az CLI](https://github.com/Azure-Samples/IoTMQTTSample/blob/master/src/AZ_CLI.JPG)


### When subscribing to IoT Hub events


Open an Azure CLI and run the following command to send a message: 
`az iot device c2d-message send --device-id <your device id> --hub-name <your IoT Hub name> --data <your message here>`

You should see something like:

![Az CLI](https://github.com/Azure-Samples/IoTMQTTSample/blob/master/src/AZ_CLI_2.JPG)
