# Using MQTT PnP with Azure IoTHub without the IoT SDK on Windows

This samples uses the Mosquitto (MQTT) library to send IoT PnP messages to an Azure IoT Hub from Windows.

This program demonstrates three PnP IoT Hub messages:

- Send a PnP device capabilities message to the IoT hub.
- Send a device twin property message to populate the device information properties.
- Send a telemetry message for a PnP telemetry data point.

## Device Capabilities Message

Upon sucessfull connection to the Azure IoT Hub, the first message send should be the DEVICE_CAPABILITIES_MESSAGE. The model information structure will describe the capability model ID of the device and the interfaces suported by the device. The device capabilities message header must contain the the model discovery header as defined below:

    $.ifid=urn:azureiot:ModelDiscovery:ModelInformation:1
    $.ifname=urn_azureiot_ModelDiscovery_ModelInformation
    $.schema=modelInformation

## Device Model Properties Message

After the device capabilities message is sent, the device should update the device twin required interfaces properties with the device model information, the device information and the client SDK information.

## Validate your code

During normal operation of the device, the device periodacly can send PnP telemetry messages. The telemetry message header is required to contain the interface ID ($.ifid) and the interface name ($.ifname) of the properties being updated.
