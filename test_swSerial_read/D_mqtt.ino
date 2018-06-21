#include "D_mqtt.h"

bool blinkState = false;

void wifiSetup(){
  int con_attempts = 0;
  int i = 0;
  delay(10);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
     
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    runEvery(4000){
      Serial.print("Connecting to "); Serial.println(ssid[i]);
      WiFi.begin(ssid[i], password[i]);
      Serial.println("Please wait for connection.");
    
      if (i < 1){ // (2 - 1)
        i++;
      }
      else{
        i = 0;
      }
    }
    yield();
  }
    
//////////////////////////
/* OLD   
  while (WiFi.status() != WL_CONNECTED) {
    if (_debug) {
      Serial.println(); Serial.print("Connecting to "); Serial.println(ssid1);
    }
    // start with ssid1
    WiFi.begin(ssid1, password1);
    
    while ((con_attempts <= 5) && (WiFi.status() != WL_CONNECTED)) {
      
      delay(2000);
      if (_debug) {
        Serial.print(con_attempts); Serial.print(" ");  
      }
      con_attempts++; 
    }
    
    if (_debug) {
      Serial.println(); Serial.print("Connecting to "); Serial.println(ssid2);  
    }
    WiFi.begin(ssid2, password2);

    while ((6 <= con_attempts ) && (con_attempts <= 10) && (WiFi.status() != WL_CONNECTED)) {
      delay(2000);
      if (_debug) {
        Serial.print(con_attempts);   
      }
      con_attempts++; 
    }
    con_attempts = 0; // reset number of retries
  }
*/

//  if (_debug) {
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("my IP address: ");
    Serial.println(WiFi.localIP());
//  }
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
      // Wait 1 seconds before retrying
      delay(1000);
    }
    yield();
  }
}


//void publishAngles(){
//  snprintf (msg, 75, "Angles: #%ld, #%ld", yaw, pitch);
//  Serial.print("Publish message: ");
//  Serial.println(msg);
//  client.publish(mqtt_rem_ang, msg);
//}

void publishCommands(){
  snprintf (msg, 75, "#%ld", "#%ld", "#%ld", "#%ld", "#%ld", dp.FwRev, dp.LeftRight, dp.Pot_P, dp.Pot_I, dp.Pot_D);
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish(mqtt_rem_cmds, msg);
}
