# Using MQTT with Azure IoT Hub Windows

These samples uses the Mosquitto (MQTT) library to send a message and subscribe to events to an Azure IoT Hub from Windows.

There are four samples in this folder:

1. **TelemetryMQTTWin32**: Send a telemetry message to an Azure IoT Hub to build and run on a Windows machine.
1. **SubscribeMQTTWin32**: Subscribe to events of a given IoT Hub on a Windows machine.
1. **DeviceTwinMQTTWin32**: Query and subscribe to the device twin events of a device in the Azure IoT Hub on a Windows machine.
1. **PnPMQTTWin32**: Send a telemetry message with PnP Device capabilities to an Azure IoT Hub to build and run on a Windows machine.

## Prerequisites

1. Follow the [general prerequisites](/README.md#general-rerequisites)
1. Install [Visual Studio](https://visualstudio.microsoft.com/downloads)
    * Enable the `Desktop development with C++` Workload.
1. Install [vcpkg](https://vcpkg.io/en/getting-started.html):

    ```cmd
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    .\vcpkg integrate install
    ```

1. Install the Mosquitto library package (this may take a while):

    ```cmd
    .\vcpkg install mosquitto:x64-windows
    ```

> **Note**
>  Check carefully if you are installing x64 or x86 version of the library. The project should target the same platform. Additional tools from mosquitto, like mosquitto_pub, are installed by vcpkg in the `{vcpkg_root_dir}\buildtrees\mosquitto\x64-windows-rel\client` folder.

## Build

1. Open the `MQTTWin32.sln` solution in Visual Studio.
1. Edit the cpp file associated with the project you wish to run and add the required connection information:

   ```c
   #define IOTHUBNAME      "{iothub_name}"
   #define DEVICEID        "{device_id}"
   #define SAS_TOKEN       "{sas_token}"
   #define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"
   ```

1. Press `CTRL-SHIFT-B` to build the entire solution

## Run

1. Right click on the project you wish to run and select `Debug | Start New Instance`

## Validate

To monitor messages at the IoT Hub, use the following command:

```Cmd
az iot hub monitor-events -n {iothub_name}
```

When subscribing to IoT Hub events on your device:

```Cmd
az iot device c2d-message send --hub-name {iothub_name} --device-id {device_id} --data "hello world"
```
