# Using the Mosquitto library

This samples uses the Mosquitto (MQTT) library to send a message to an Azure IoT Hub.

## Windows prerequisites

1. Follow the [general prerequisites](/README.md#general-rerequisites)
1. Install [Visual Studio](https://visualstudio.microsoft.com/downloads)
 
    > **NOTE**
    > Enable the `Desktop development with C++` workload

1. Install [CMake](https://cmake.org/download/)

    > **NOTE**
    > Enable the `Add CMake to the system PATH for all users` option

1. Install the **x64 version** of [Mosquitto](https://mosquitto.org/download/)

## Linux prerequisites

1. Follow the [general prerequisites](/README.md#general-rerequisites)
1. Install the build tools:

    ```Shell
   sudo apt install cmake g++
    ```

1. Install Mosquitto client library:

    ```Shell
    sudo apt install libmosquitto-dev
    ```

## Variable substitions

The following variables will need to be expanded in the samples:

| Variable | Description | Example |
|-|-|-|
| iothub_name | The name of the created IoT Hub (**NOT the FQDN/HostName**) | *iothub*
| device_id | The name of the device created in the IoT Hub | *mosquitto_pub*
| sas_token | The SAS token [generated](https://learn.microsoft.com/cli/azure/iot/hub?view=azure-cli-latest#az-iot-hub-generate-sas-token) for the device | *SharedAccessSignature sr=\*.azure-devices.net%2Fdevices%2F\*&sig=\**

## Build

1. Change to the `mosquitto` directory in the cloned repository

    ```Shell
    cd mosquitto
    ```
 
1. Edit the `config.h` file and expand the required connection information:

   ```c
   #define IOTHUBNAME      "{iothub_name}"
   #define DEVICEID        "{device_id}"
   #define SAS_TOKEN       "{sas_token}"
   ```

1. Build the application:

    ```Shell
    cmake -Bbuild
    cmake --build build
    ```

1. Resulting binaries can be found in the following locations:
    1. Linux: `./build`
    1. Windows `.\build\Debug`

## Run

### Telemetry sample

This sample will send a single telemetry message, which can be viewed by monitoring the Hubs events.

1. Start monitoring incoming messages on your Hub:

    ```Shell
    az iot hub monitor-events -n {iothub_name}
    ```

1. Run the sample executable:

    ```Shell
    mosquitto_telemetry
    ```

1. Confirm the following output is seen from monitor-events:

    ```json
    {
        "event": {
            "origin": "*",
            "module": "",
            "interface": "",
            "component": "",
            "payload": "Bonjour MQTT from Mosquitto"
        }
    }
    ```

### Subscribe sample

This sample will subscribe to all available MQTT topics, and then display an cloud-to-device message sent from the Hub.

1. Run the sample executable:

    ```Shell
    mosquitto_subscribe
    ```

1. Send an event from the Hub to the device:

    ```Shell
    az iot device c2d-message send --hub-name {iothub_name} --device-id {device_id} --data "hello world"
    ```

1. Confirm the following output from the sample:

    ```Shell
    Waiting for C2D messages...
    C2D message 'hello world' for topic 'devices/*/messages/devicebound/...`
    Got message for devices/*/messages/# topic
    ```

### Device twin sample

This sample will add a temperature reported property to the device twin, and then read the property back.

1. Run the sample executable:

    ```Shell
    mosquitto_device_twin
    ```

1. Confirm the following output from the sample, setting and getting a device twin update:

    ```Shell
    Setting device twin reported properties....
    Device twin message '' for topic '$iothub/twin/res/204/?$rid=patch_temp&$version=2'
    Setting device twin properties SUCCEEDED.
    
    Getting device twin properties....
    Device twin message '{"desired":{"$version":1},"reported":{"temperature":32,"$version":2}}' for topic '$iothub/twin/res/200/?$rid=123'
    Getting device twin properties SUCCEEDED.
    ```
