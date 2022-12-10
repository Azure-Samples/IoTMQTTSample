# Mosquitto_pub CLI samples

This document contains two samples commands using the `mosquitto_pub` CLI from [Mosquitto](https://mosquitto.org).

1. Send a simple text mesaage to an Azure IoTHub acting as a device
1. See events within an Azure IoT Hub

## Prerequisites

The following steps must be complete before running the samples below:

1. Follow the [general prerequisites](/README.md#general-rerequisites)
1. Install [Mosquitto](https://mosquitto.org/download) to your target machine
1. Confirm that `mosquitto_pub` is in your system path

## Variable substitution

The following variables will need to be expanded in the following samples:

| Variable | Description |
|-|-|
| iothub_name | The name of the created IoT Hub |
| device_id | The name of the device created in the IoT Hub |
| sas_token | The SAS token generated for the device |
| certificate_pem | The [Root CA](/README.md#root-certificates) used to validate the IoT Hub |

## Send a message

1. Change to the `src` directory in the cloned repository directory for easier access to the root certificates

1. Start monitoring incoming messages on your Hub:

    ```Shell
    az iot hub monitor-events -n {iothub_name}
    ```

1. From a second terminal, send a message from the device:

    ```Shell
    mosquitto_pub -d -h {iothub_name}.azure-devices.net -p 8883 -i {device_id} -u "{iothub_name}.azure-devices.net/{device_id}/?api-version=2018-06-30" -P "{sas_token}>" -t "devices/{device_id}/messages/events/" -m "hello world" -V mqttv311 --cafile {certificate_pem} -q 1
    ```

3. Confirm the following output is seen from the monitor-events command:
    
    ```json
    {
        "event": {
            "origin": "pubsub",
            "module": "",
            "interface": "",
            "component": "",
            "payload": "hello world"
        }
    }
    ```

## Subscribe to events

1. Subscribe the device to events

    ```Shell
    mosquitto_sub -d -h {iothub_name}.azure-devices.net -p 8883 -i {device_id} -u "{iothub_name}.azure-devices.net/{device_id}/?api-version=2018-06-30" -P "{sas_token}" -t "#" -V mqttv311 --cafile {certificate_pem} -q 1
    ```

2. Send a message from the Hub to the device:

    ```Shell
    az iot device c2d-message send --hub-name {iothub_name} --device-id {device_id} --data "hello world"
    ```

3. Confirm the following output from the mosquitto_sub command:

    ```Output
    Client pubsub received PUBLISH (d0, q1, r0, m2, 'devices/pubsub/messages/devicebound/%24.mid=2c2db32a-9c41-4073-baa4-db584e021573&%24.to=%2Fdevices%2Fpubsub%2Fmessages%2Fdevicebound&%24.ce=utf-8&iothub-ack=none', ... (11 bytes))  
    Client pubsub sending PUBACK (m2, rc0)
    hello world
    ```
