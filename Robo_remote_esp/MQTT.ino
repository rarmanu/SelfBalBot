#include "mqttHelpers.h"
#include <ESP8266WiFi.h>
//#include <WiFiClient.h> 
#include <PubSubClient.h>


///* Credentials for AP connection. */
//const char* ssid = "AI-THINKER_EDE868";
//const char* password = "";
//
//// Raspberry py (broker) address
//const char* mqtt_server = "192.168.1.82"; 
//// Topics:
////const char* mqtt_topic = "Your Topic";
//const char* mqtt_log = "topic/log";
//const char* mqtt_rem_ang = "topic/angles";
//
//
//const char* mqtt_username = "pi";         // "MQTT Username";
//const char* mqtt_password = "raspberry";  // "MQTT Password";
//
//WiFiClient espClient;
//PubSubClient client(espClient);
//char msg[50];


void wifiSetup(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("my IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    blinkState = !blinkState;
    digitalWrite(BUILTIN_LED, blinkState);   // Turn the LED on/off 
    
  } 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_log, "[ESP] Remote reconnected");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void publishAngles(){
  snprintf (msg, 75, "Angles: #%ld, #%ld", yaw, pitch);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(mqtt_rem_ang, msg);
}

