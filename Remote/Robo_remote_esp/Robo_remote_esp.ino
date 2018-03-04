/* Remote control for self balancing robot
- ESP8266 12e (NodeMCU)
- Arduino Nano
- MPU6050 (i2c accelerometer & gyroscope)

ESP is the i2c master. It reads data from MPU6050 and process them.
There is an i2c link between Nano and ESP. 
ESP is running MQTT.

*/

#include "mqttHelpers.h"
#include "i2cHelpers.h";
#include <Wire.h>  // D2=SDA; D1=CLK

#include <ESP8266WiFi.h>
//#include <WiFiClient.h> 
#include <PubSubClient.h>


// Rutine for repeat part of the code every X miliseconds
#define runEvery(t) for (static long _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))

uint32_t timer; 
bool blinkState = false;

bool low_bat = false;    // batery low voltage detection

double yaw, pitch;


void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  delay(10);
  
  Serial.println(" WiFi setup... ");
  wifiSetup();
//  MQTT setup
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println(" Arduino Nano i2c comms setup... ");
  setup_nano_i2c();

  Serial.println(" MPU6050 setup... ");
  mpu_setup();

  timer = micros(); //start timer
}

void loop() {
  // MQTT client
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // get angles data
  runEvery(100){
    get_angles();
    yaw   =   get_angX();
    Serial.print("yaw= "); Serial.print(yaw);
    pitch =   get_angY();
    Serial.print("\tpitch= "); Serial.println(pitch);

    // i2c comms with Arduino Nano:
    write_2_nano();

    request_from_nano();

    Serial.print("received i2c watchdog = "); Serial.println(r_i2c_wd);
    
  }

  // MQTT publish Angles
  runEvery(200){
    publishAngles();

  
  }


}

