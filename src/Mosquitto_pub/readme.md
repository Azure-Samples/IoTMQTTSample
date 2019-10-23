# Mosquitto_pub commands sample
This folder contains two samples commands used with mosquitto_pub utility tool provided by Mosquitto.org.

Mosquitto_sendmessage: to send a simple text mesaage to an Azure IoTHub acting as a device.

Mosquitto_subscribe: to see events occurring in an Azure IoT Hub.

## Setup
Install Mosquitto lib on your machine. These 2 commands can be used on Windows or Linux.
Use apt-get on Linux and vcpkg on windows to install the tool (see readme in Linux and Windows directory to learn how to setup Mosquitto on the platform you want)

## Run
Update the SAS token in the command. (see readme at the root at this repository to learn how to generate a SAS token for your IoTHub)
Update the IoThub name <MyHub> and device name <MyDevice>
Copy and paste the command to a command line and run it.

send a message:

`mosquitto_pub -d -h <MyHub>.azure-devices.net -p 8883 -i <MyDevice> -u "<MyHub>.azure-devices.net/<MyDevice>/?api-version=2018-06-30" -P "<SAS Token for <MyDevice>" -t "devices/<MyDevice>/messages/events/" -m "hello world" -V mqttv311 --cafile <Certificate.pem> -q 1`


subscribe to events:

`mosquitto_sub -d -h <MyHUB>.azure-devices.net -p 8883 -i EM_MXC3166 -u "<MyHUB>.azure-devices.net/<DEviceId>/?api-version=2018-06-30" -P "SharedAccessSignature sr=<MyHUB>.azure-devices.net%2Fdevices%2F<DeviceId>&sig=T768rIqQVq6zVBL7FeWAYqak7tTeslu%MCMG639zX492o%3D&se=1568424250" -t "#"  -V mqttv311 --cafile IoTHubRootCA_Baltimore.pem -q 1`

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
