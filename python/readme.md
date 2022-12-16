
# Using MQTT to connect an IoT Hub with Python

## Prerequisites

1. Follow the [general prerequisites](/README.md#general-rerequisites)
1. Install [Python](https://www.python.org/downloads)
1. Install [Pip](https://pip.pypa.io/en/stable/installation)

## Variable substitution

The following variables will need to be expanded in the sample:

| Variable | Description | Example |
|-|-|-|
| iothub_name | The name of the created IoT Hub (**NOT FQDN/HostName**) | *iothub*
| device_id | The name of the device created in the IoT Hub | *mosquitto_pub*
| sas_token | The SAS token [generated](https://learn.microsoft.com/cli/azure/iot/hub?view=azure-cli-latest#az-iot-hub-generate-sas-token) for the device | *SharedAccessSignature sr=\*.azure-devices.net%2Fdevices%2F\*&sig=\**

## Setup

1. Open main.py and edit the following fields with the correct IoT Hub and device information
    * IOT_HUB_NAME
    * IOT_HUB_DEVICE_ID
    * IOT_HUB_SAS_TOKEN

1. Install Paho MQTT

    ```Shell
    pip install paho-mqtt
    ```

1. Change into the `python` directory  in the cloned repository:

    ```Shell
    cd python
    ```

1. Run the script

    ```Shell
    python main.py
    ```

1. Observe the telemetry being sent

    ```Shell
    sending message[0]: Accio
    connect: 0
    publish: 1
    sending message[1]: Aguamenti
    publish: 2
    sending message[2]: Alarte Ascendare
    publish: 3
    sending message[3]: Expecto Patronum
    publish: 4
    sending message[4]: Homenum Revelio
    publish: 5
    ```
