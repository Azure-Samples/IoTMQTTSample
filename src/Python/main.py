import paho.mqtt.client as mqtt
from base64 import b64encode, b64decode
from time import time, sleep
from urllib import parse
from hmac import HMAC
from hashlib import sha256
from ssl import SSLContext, PROTOCOL_TLS_CLIENT, CERT_REQUIRED

IOT_HUB_HOSTNAME  = "azuregsg.azure-devices.net"
IOT_HUB_DEVICE_ID = "python2"
IOT_HUB_SAS_KEY   = "YTMffA4TW0SHZ5fXmoySuKzWYifrnK0Y0NOqaQaan38="

def on_connect(mqtt_client, obj, flags, rc):
    print("connect: " + str(rc))

def on_publish(mqtt_client, obj, mid):
    print("publish: " + str(mid))

def generate_sas_token():
    ttl = int(time()) + 300
    uri = IOT_HUB_HOSTNAME + "/devices/" + IOT_HUB_DEVICE_ID
    sign_key = uri + "\n" + str(ttl)

    signature = b64encode(HMAC(b64decode(IOT_HUB_SAS_KEY), sign_key.encode('utf-8'), sha256).digest())

    return "SharedAccessSignature sr=" + uri + "&sig=" + parse.quote(signature, safe="") + "&se=" + str(ttl)

mqtt_client = mqtt.Client(client_id=IOT_HUB_DEVICE_ID, protocol=mqtt.MQTTv311)
mqtt_client.on_connect    = on_connect
mqtt_client.on_publish    = on_publish

mqtt_client.username_pw_set(username=IOT_HUB_HOSTNAME + "/" + IOT_HUB_DEVICE_ID + "/?api-version=2021-04-12", 
                            password=generate_sas_token())

ssl_context = SSLContext(protocol=PROTOCOL_TLS_CLIENT)
ssl_context.load_default_certs()
ssl_context.verify_mode = CERT_REQUIRED
ssl_context.check_hostname = True
mqtt_client.tls_set_context(context=ssl_context)

mqtt_client.connect(host=IOT_HUB_HOSTNAME, port=8883, keepalive=120)

# start the MQTT processing loop
mqtt_client.loop_start()

# send telemetry
messages = ["Accio", "Aguamenti", "Alarte Ascendare", "Expecto Patronum", "Homenum Revelio"]
for i in range(0, len(messages)):
    print("sending message[" + str(i) + "]: " + messages[i])
    mqtt_client.publish("devices/" + IOT_HUB_DEVICE_ID + "/messages/events/", payload=messages[i], qos=1)
    sleep(1)