# Using the Mosquitto library on Linux

This samples uses the Mosquitto (MQTT) library to send a message to an Azure IoT Hub from a Linux machine.

## Prerequisites

The following steps must be complete before running the samples below:

1. Follow the [general prerequisites](/README.md#general-rerequisites)
1. Install Mosquitto client library:

    ```Shell
    sudo apt install libmosquitto-dev
    ```

1. Install the c++ compiler:

    ```Shell
   sudo apt install g++
    ```

## Build

1. Change to the `src` directory in the cloned repository
1. Edit the `Shared/MQTT_Mosquitto.cpp` file and add the required connection information:

   ```c
   #define IOTHUBNAME      "{iothub_name}"
   #define DEVICEID        "{device_id}"
   #define SAS_TOKEN       "{sas_token}"
   #define CERTIFICATEFILE "IoTHubRootCA_Baltimore.pem"
   ```

1. Build the application:
   > `g++ Shared/MQTT_Mosquitto.cpp -lmosquitto -o mqttlinux`

## Run

1. Start monitoring incoming messages on your Hub:

    ```Shell
    az iot hub monitor-events -n {iothub_name}
    ```

1. Run the built executable:

    ```Shell
    ./mqttlinux
    ```

1. Confirm the following output is seen from the monitor-events command:

    ```json
    {
        "event": {
            "origin": "pubsub",
            "module": "",
            "interface": "",
            "component": "",
            "payload": "Bonjour MQTT from Mosquitto"
        }
    }
    ```
