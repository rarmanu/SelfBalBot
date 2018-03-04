#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

#ifndef MQTTHELPERS_H
#define MQTTHELPERS_H


  /* Credentials for AP connection. */
  const char* ssid = "AI-THINKER_EDE868";
  const char* password = "";
  
  // Raspberry py (broker) address
  const char* mqtt_server = "192.168.1.83"; 
  // Topics:
  //const char* mqtt_topic = "Your Topic";
  const char* mqtt_log = "topic/log";
  const char* mqtt_rem_ang = "topic/angles";
  
  
  const char* mqtt_username = "pi";         // "MQTT Username";
  const char* mqtt_password = "raspberry";  // "MQTT Password";
  
  WiFiClient espClient;
  PubSubClient client(espClient);
  char msg[50];

#endif
