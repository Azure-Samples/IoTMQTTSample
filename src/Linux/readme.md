# Using MQTT with Azure IoTHub without SDK on Linux

These samples use the Mosquitto (MQTT) library to send a message to an Azure IoT Hub from a Linux machine.

There are two samples in this folder:

- MQTTLinux: contains code and build script to run on Linux (WSL, Ubuntu and Raspbian have been tested so far).

- LinuxConsoleVS2019: contains the same code but in a VS2019 project targeting WSL (Windows Linux sub system). This project allows you to debug the code running on Linux step by step from Visual Studio.

## Prerequisite

You need to install Mosquitto library (client only, not the server/broker), on Linux use:

`sudo apt install libmosquitto-dev`

For the VS2019 console application, you need to install the latest version of [Visual Studio 2019](https://visualstudio.microsoft.com/downloads/).

> Select "Desktop Development with C++" and "Linux development with C++" workloads in the Visual Studio installer.

## Setup

Clone the current [github repository]( https://github.com/Azure-Samples/IoTMQTTSample) on your machine.

### For Windows Subsystem Linux

Open a new WSL command line on Windows (type wsl in any cmd line Windows or directly from the start menu).

In case WSL is not yet installed on your machine, install it and configure the latest g++ tool chain.

## WSL

See [this article](https://docs.microsoft.com/en-us/windows/wsl/install-win10) for more information.

### For Linux distro, including Raspbian for RaspberryPi

Navigate to the {root}\IoTMQTTSample\src\Linux\MQTTLinux directory.

## Running the code

The code can be built for different Linux distros.

Scenarios verified so far include:

- WSL (Windows Subsystem Linux on Windows 10 with Ubuntu)
- Raspberry Pi (Raspbian)

### Build the code sample with g++

1. Install the c++ compiler, g++, with the command `sudo apt install g++`

1. Be sure your SAS Token is up to date, as the SAS Token has a limited valid lifetime. If necessary generate a new one and copy/paste it in the source code (mqtt_mosquitto.cpp line 11). See how to generate a SAS token in [readme.md](../../readme.md) at the root of the repository.

1. To build from the command line run `./build.sh`

   > The script includes all the necessary parameters for you: `g++ mqttlinux.cpp -lmosquitto -o mqttlinux`

1. Then run the built executable `./mqttlinux`

> Note: Use the exact same build process to build on Rasbian for Raspberry Pi.

### Build and debug with Visual Studio 2019 from Windows10

1. Open the project MQTTWSL.sln placed in \LinuxConsoleVS2019.

   Or

   Create a new "Linux console application" project and set the tool chain to **WSL_1_0** and set the platform to **localhost/x64** see [this article](https://devblogs.microsoft.com/cppblog/c-with-visual-studio-2019-and-windows-subsystem-for-linux-wsl/) to learn more about WSL and Visual Studio 2019.

1. Put couple of breakpoints and start the debug session (press F5).

## Validate your code

### When sending message

Open an Azure CLI prompt and run the command `az iot hub monitor-events --hub-name <your IoT Hub name> --output table`

You should see something like:

![Az CLI](https://github.com/Azure-Samples/IoTMQTTSample/blob/master/src/AZ_CLI.JPG)
