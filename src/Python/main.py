import paho.mqtt.client as mqtt
import base64
import hashlib
import hmac
import ssl
import time
import urllib

IOT_HUB_HOSTNAME  = "<hostname>"
IOT_HUB_DEVICE_ID = "<device id>"
IOT_HUB_SAS_KEY   = "<sas key>"

def on_connect(mqtt_client, obj, flags, rc):
    print("connect: " + str(rc))

def on_disconnect(mqtt_client, obj, rc):
    print("disconnect: " + str(rc))

def on_publish(mqtt_client, obj, mid):
    print("publish: " + str(mid))

def create_signature():
    # create expiry time
    valid_until = str(int(time.time()) + 300)
    device = IOT_HUB_HOSTNAME + "/devices/" + IOT_HUB_DEVICE_ID

    # base64 decode the sas_key
    signing_key = base64.b64decode(IOT_HUB_SAS_KEY)

    # sign using HMAC sha256
    data = device + "\n" + valid_until
    data = data.encode("utf-8")
    hmac_digest = hmac.HMAC(key=signing_key, msg=data, digestmod=hashlib.sha256).digest()

    # base64 encode
    signed_data = base64.b64encode(hmac_digest)
    url_encoded_signed_data = urllib.parse.quote(signed_data, safe="")

    return "SharedAccessSignature sr=" + device + "&sig=" + url_encoded_signed_data + "&se=" + valid_until

mqtt_client = mqtt.Client(client_id=IOT_HUB_DEVICE_ID, protocol=mqtt.MQTTv311)
mqtt_client.on_connect    = on_connect
mqtt_client.on_disconnect = on_disconnect
mqtt_client.on_publish    = on_publish

mqtt_client.username_pw_set(username=IOT_HUB_HOSTNAME + "/" + IOT_HUB_DEVICE_ID, 
                            password=create_signature())

ssl_context = ssl.SSLContext(protocol=ssl.PROTOCOL_TLS_CLIENT)
ssl_context.load_default_certs()
ssl_context.verify_mode = ssl.CERT_REQUIRED
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
    time.sleep(1)