## The file name needs to be renamed to main.py for it work on the ESP 32 board

import utime
from util import create_mqtt_client, get_telemetry_topic, get_c2d_topic, parse_connection

HOST_NAME = "HostName"
SHARED_ACCESS_KEY_NAME = "SharedAccessKeyName"
SHARED_ACCESS_KEY = "SharedAccessKey"
SHARED_ACCESS_SIGNATURE = "SharedAccessSignature"
DEVICE_ID = "DeviceId"
MODULE_ID = "ModuleId"
GATEWAY_HOST_NAME = "GatewayHostName"


## Parse the connection string into constituent parts
dict_keys = parse_connection("<YOUR CONNECTION STRING>")
shared_access_key = dict_keys.get(SHARED_ACCESS_KEY)
shared_access_key_name =  dict_keys.get(SHARED_ACCESS_KEY_NAME)
gateway_hostname = dict_keys.get(GATEWAY_HOST_NAME)
hostname = dict_keys.get(HOST_NAME)
device_id = dict_keys.get(DEVICE_ID)
module_id = dict_keys.get(MODULE_ID)

## Create you own shared access signature from the connection string that you have
## Azure IoT Explorer can be used for this purpose.
sas_token_str = "<YOUR SAS TOKEN STRING>"

## Create username following the below format '<HOSTNAME>/<DEVICE_ID>'
username = hostname + '/' + device_id


## Create UMQTT ROBUST or UMQTT SIMPLE CLIENT
mqtt_client = create_mqtt_client(client_id=device_id, hostname=hostname, username=username, password=sas_token_str, port=8883, keepalive=120, ssl=True)

print("connecting")
mqtt_client.reconnect()

def callback_handler(topic, message_receive):
    print("Received message")
    print(message_receive)

subscribe_topic = get_c2d_topic(device_id)
mqtt_client.set_callback(callback_handler)
mqtt_client.subscribe(topic=subscribe_topic)

print("Publishing")
topic = get_telemetry_topic(device_id)

## Send telemetry
messages = ["Accio", "Aguamenti", "Alarte Ascendare", "Expecto Patronum", "Homenum Revelio", "Priori Incantato", "Revelio", "Rictusempra",  "Nox" , "Stupefy", "Wingardium Leviosa"]
for i in range(0, len(messages)):
    print("Sending message " + str(i))
    mqtt_client.publish(topic=topic, msg=messages[i])
    utime.sleep(2)

## Send a C2D message and wait for it to arrive at the device
print("waiting for message")
mqtt_client.wait_msg()


