"""
Python MQTT Subscription client
Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
Written for my Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
"""
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time


# Don't forget to change the variables for the MQTT broker!
mqtt_username = "pi"
mqtt_password = "raspberry"
mqtt_topic_in = "inTopic"  # ESP is receiving (subscibing)
mqtt_topic_out = "outTopic" # ESP is sending (publishing)
mqtt_topic_time = "topic/time"
mqtt_topic_log = "topic/log"
#mqtt_broker_ip = "192.168.1.220"

received_msg = ""  # empty string

client = mqtt.Client()
# Set the username and password for the MQTT client
client.username_pw_set(mqtt_username, mqtt_password)

# These functions handle what happens when the MQTT client connects
# to the broker, and what happens then the topic receives a message
def on_connect(client, userdata, flags, rc):
    # rc is the error code returned when connecting to the broker
    print("Connected with result code ", str(rc))
    
    # Once the client has connected to the broker, subscribe to the topic
    client.subscribe(mqtt_topic_out)
    
def on_message(client, userdata, msg):
    # This function is called everytime the topic is published to.
    # If you want to check each message, and do something depending on
    # the content, the code to do this should be run in this function
    
    if msg.topic == mqtt_topic_log:
        print("Topic: ", msg.topic + " " + str(msg.qos) + "\nMessage: " + str(msg.payload))
    
        
    
    
    print("Topic: ", msg.topic + " " + str(msg.qos) + "\nMessage: " + str(msg.payload))
    
    # The message itself is stored in the msg variable
    # and details about who sent it are stored in userdata

    if msg.topic == mqtt_topic_out:
        received_msg = msg.payload
        
def on_publish(client, userdata, rc):
    print("mid: " + str(rc))  # prints each time it publishes something
 
    pass


def on_subscribe(client, userdata, mid, granted_qos):
    # prints each time it's subscibing to a topic
    print("Subscribed: " + str(userdata) + " " + str(granted_qos))

    
def on_log(client, userdata, level, string):
    print(string)
   

# Here, we are telling the client which functions are to be run
# on connecting, and on receiving a message
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish
client.on_subscribe = on_subscribe
# Uncomment to enable debug messages
# client.on_log = on_log

# Once everything has been set up, we can (finally) connect to the broker
# 1883 is the listener port that the MQTT broker is using
##client.connect(mqtt_broker_ip, 1883)
client.connect('localhost', 1883, 60) # connect(host, port=1883, keepalive=60, bind address="") 

time.sleep(1)
print("I am sending")
ret = client.publish(mqtt_topic_in, "1")

# Once we have told the client to connect, let the client object run itself
#client.loop_forever()
client.loop_start()
print('Script is running, press Ctrl+c to quit...')

while True:
    
    ret = client.publish(mqtt_topic_in, "1")
    time.sleep(1)

client.loop_stop()
client.disconnect()
