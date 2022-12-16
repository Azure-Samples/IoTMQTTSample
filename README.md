---
page_type: sample
languages:
  - c
  - python
products:
  - azure-iot-hub
name: "IoT MQTT Samples"
description: "Using MQTT with Azure IoTHub without SDK"
---

# Using MQTT with Azure IoT Hub without an SDK

This set of samples will demonstrate how to connect and send messages to an Azure IoT Hub without using the Azure IoT SDKs.

Read the [Communicate with your IoT Hub using the MQTT protocol](https://docs.microsoft.com/azure/iot-hub/iot-hub-mqtt-support) for detailed instructions on connecting your device to IoT Hub using MQTT without an SDK.

## Why use an Azure IoT Device SDK?

Azure provides a set of SDKs across multiple languages for connecting device to [IoT Hub](https://docs.microsoft.com/azure/iot-hub/iot-concepts-and-iot-hub) and [DPS](https://docs.microsoft.com/azure/iot-dps). The advantages of using an [Azure IoT Device SDK](https://docs.microsoft.com/azure/iot-develop/about-iot-sdks) over building a custom connection layer are outlined below:

| | Custom connection layer | Azure IoT Device SDKs |
| :-- | :-- | :-- |
| **Support** | Need to support and document your solution | Access to Microsoft support (GitHub, Microsoft Q&A, Microsoft technical documentation, Customer Support teams) |
| **New Features** | Need to manually add new Azure features | Can immediately take advantage of new features added |
| **Investment** | Invest hundreds of hours of embedded development to design, build, test, and maintain a custom version | Can take advantage of free, open-source tools. The only cost associated with the SDKs is the learning curve. |

For more information, refer to the [Overview of Azure IoT Device SDKs](https://docs.microsoft.com/azure/iot-develop/about-iot-sdks).

## Samples in this repo

This repository contains the following samples:

* [Mosquitto client library](mosquitto)
* [Mosquitto_pub CLI](mosquitto_pub)
* [Python](python)

## Root certificates

> **Note**
> Azure IoT services are moving to a new CA root, see [here](http://aka.ms/iot-ca-updates) for details.

The following root certificates are provided in the repository root:

| Certificate | Path | Description |
|-|-|-|
| [Baltimore CyberTrust Root](https://www.digicert.com/kb/digicert-root-certificates.htm) | BaltimoreCyberTrustRoot.crt.pem | **NOT recommended for use** since [IoT Hub migration out of Baltimore](https://techcommunity.microsoft.com/t5/internet-of-things-blog/azure-iot-tls-critical-changes-are-almost-here-and-why-you/ba-p/2393169) is set to start on Feb 2023.
| [DigiCert Global Root G2](https://www.digicert.com/kb/digicert-root-certificates.htm) | DigiCertGlobalRootG2.crt.pem | Future Root CA which will become active in Feb 2023 |

## General Prerequisites

To be able to run these samples the following general prerequisites are needed:

1. Clone this repository (Install [Git](https://git-scm.com/downloads))
1. Change into the cloned directory:

```
    cd IoTMQTTSample
```

1. Install the [Azure CLI](https://learn.microsoft.com/en-us/cli/azure/install-azure-cli)
1. [Provision an IoT Hub](https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal) in your Azure subscription
1. [Register a device](https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-create-through-portal#register-a-new-device-in-the-iot-hub) within your IoT Hub
1. [Generate a SAS token](https://learn.microsoft.com/cli/azure/iot/hub?view=azure-cli-latest#az-iot-hub-generate-sas-token) for the device using the Azure CLI

    > **Note**
    > By default the **SAS Token is valid for 60 minutes**. Use the --du parameter to increase if needed.

Additional prerequisites may be required by each individual sample.

## Contributing

For details on contributing to this repository, see the [contributing](CONTRIBUTING.md) guide.

## Reporting Security Vulnerabilities

If you believe you have found a security vulnerability in any Microsoft-owned repository that meets Microsoft's definition of a security vulnerability, please report it to the [Microsoft Security Response Center](SECURITY.md).
