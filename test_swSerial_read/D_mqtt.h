#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

#ifndef MQTT_H
#define MQTT_H

/* Credentials for router connection. */
  const char* ssid1 = "Nihaha";
  const char* password1 = "Galop123";

  const char* ssid[] = {"Nihaha" , "AI-THINKER_EDE868"};
  const char* password[] = {"Galop123" , ""};
  
  /* Credentials for AP connection. */
  const char* ssid2 = "AI-THINKER_EDE868";
  const char* password2 = "";

  // Raspberry py (broker) address
  const char* mqtt_server = "192.168.1.83"; 
  const char* mqtt_username = "pi";         // "MQTT Username";
  const char* mqtt_password = "raspberry";  // "MQTT Password";
  
  // Topics:
  //const char* mqtt_topic = "Your Topic";
  const char* mqtt_log = "topic/log";
  const char* mqtt_rem_ang = "topic/angles";
  const char* mqtt_rem_cmds = "topic/remote";
  
  WiFiClient espClient;
  //PubSubClient client(espClient);
  PubSubClient client(mqtt_server, 1883, espClient);
  char msg[50];

#endif
