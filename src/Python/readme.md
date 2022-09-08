
# Using MQTT to connect an IoT Hub with Python

## Assumptions
1. Python is [installed](https://www.python.org/downloads)
1. Pip is [installed](https://pip.pypa.io/en/stable/installation)
1. An IoT Hub has been [created](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal)
1. A device has been [registered](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal#register-a-new-device-in-the-iot-hub) using Symmetric key authentication

## Setup
1. Open main.py and edit the following fields with the correct IoT Hub and device information
    * IOT_HUB_HOSTNAME
    * IOT_HUB_DEVICE_ID
    * IOT_HUB_SAS_KEY

2. Install Paho MQTT

```
pip install paho-mqtt
```

3. Run the script

```
python main.py
```

4. Observe the telemetry being sent
```
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