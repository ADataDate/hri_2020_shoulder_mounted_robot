#!/usr/bin/env python3
import serial
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import json
import os
import urllib.parse as urlparse
import time 

# Environment variable, run command when using a new terminal
# export CLOUDMQTT_URL=mqtt://rwjanqog:0wo-qFBN9zOs@soldier.cloudmqtt.com:16801

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# process a message when recived
def on_message(client, userdata, msg):
    print(msg.payload)
    '''TODO: add decision on emotion and tx'''


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("connected OK")
    else:
        print("Bad connection return code = ", rc)

def on_publish(client, userdata, result):
#	print("data published \n")
	pass

# Note: client name must be unique across devices
client_name = "batbot1"
#broker = "mqtt://rwjanqog:0wo-qFBN9zOs@soldier.cloudmqtt.com:16801"
client = mqtt.Client(client_name)


url_str = os.environ.get("CLOUDMQTT_URL")
print("If the end of this line doesn't have the URL run command to set environment variable: {}".format(url_str))
url = urlparse.urlparse(url_str)
client.username_pw_set(url.username, url.password)
client.connect(url.hostname, url.port)
#client.connect_async(url.hostname, url.port)


# Add all methods defined at the start of the file to the client
client.on_subscribe = on_subscribe
client.on_message = on_message
client.on_connect = on_connect
client.on_publish = on_publish
client.subscribe("austin/eye/emotion", qos=1)

#client.loop_forever()
client.loop_start()

# Read sensor data from Arduino serial
ser = serial.Serial('/dev/ttyUSB1', 9600)
print(ser.name)

#Read instruction line: "Attach Finger to sensor" but do not send
print(ser.readline())

while 1:
    line = ser.readline()
    line = line.decode("utf-8") #convert line to string
    #print(line)
    #Publish data after single read
    client.publish("bio_sensors/sensors/sensor_data", line)
    time.sleep(0.1)

ser.close()

''' TODO:
Subscribe to 'mild stress' data to activate deep breathe exercise vibrotactile element
Subscribe to high stress data to activate DTPT vest 

'''