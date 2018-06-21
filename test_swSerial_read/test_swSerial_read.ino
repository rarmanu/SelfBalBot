/*  Read data from arduino over sofware serial
 *  Publish data over wifi (mqtt)
 * 
 */

/* ESP8266 12e pinout
 *  D0   =    16; 
    D1   =    5;
    D2   =    4;
    D3   =    0;
    D4   =    2; 
    D5   =    14;
    D6   =    12;
    D7   =    13;
    D8   =    15;
    D9   =    3;
    D10  =    1;
 */

#include "Arduino.h"
#include <uCRC16Lib.h>
//#include <ESP8266WiFi.h> 
//#include <PubSubClient.h>
#include <EspSoftwareSerial.h>
SoftwareSerial EspSerial(14, 12, false, 256); // D5-RX, D6-TX

// Rutine for repeat part of the code every X miliseconds
#define runEvery(t) for (static long _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))
uint32_t timer;
int _debug = true;  // enable/disable Serial.print

// define serial data pachet struct:
/*
 * not used yet
typedef struct {
 int8_t id;
 int16_t messageCount;
 int16_t time;
} __attribute__((__packed__))packet_header_t;
*/
typedef struct {
 //packet_header_t header;
 int16_t FwRev;     // joystick Fw / Rev
 int16_t LeftRight; // joystick Left / Right
 int16_t Pot_P;     // PID P param
 int16_t Pot_I;     // PID I param
 int16_t Pot_D;     // PID D param
 int16_t crc;
} __attribute__((__packed__))data_packet_t;

data_packet_t dp;
data_packet_t temp_dp; // before checking CRC
int uBufSize = sizeof(data_packet_t);
char pBuffer[14];  // sizeof(data_packet_t) + 2 (for line return character)
//char pBuffer[(uBufSize+2)];
int missed_packs_tot = 0;
int missed_packs_count = 0;
int new_data = false;

bool RecvWithEndMarker(void);
void HandleNewData(void);
int16_t calcCRC(void);
void wifiSetup();


void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  delay(10);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (_debug) {
    Serial.println("Serial OK!");
  }

  // set the data rate for the SoftwareSerial port
  EspSerial.begin(9600); 
  delay(10);
  if (_debug) {
    Serial.println("ESP Serial OK!");
  }
  if (_debug) {  
    Serial.println(" WiFi setup... ");
  }
  
  wifiSetup();
  delay(500);
  
  //dp.header.id = 99;
  //dp.header.messageCount = 0;
  //dp.header.time = 1;
  dp.FwRev = 0;
  dp.LeftRight = 0;
  dp.Pot_P = 0;
  dp.Pot_I = 0;
  dp.Pot_D = 0;
  dp.crc = 1;
  
  timer = micros(); //start timer
}

void loop() {

  //RecvWithEndMarker();
  if (RecvWithEndMarker() && new_data) {
    HandleNewData();  
  }
}

/*
//void RecvWithEndMarker() {
//  static byte ndx = 0;
//  char endMarker = '\n';
//  char rc;
//
//  while (EspSerial.available() > 0 && new_data == false) {
//    rc = EspSerial.read();
//    if (rc != endMarker) {
//      pBuffer[ndx] = rc;
//      ndx++;
//      if (ndx >= uBufSize) {
//        ndx = uBufSize; // - 1;
//      }
//    }
//    else {
//      pBuffer[ndx] = '\0'; // terminate the string
//      ndx = 0;
//      new_data = true;
//    }
//    yield();
//  }
//}
*/

bool RecvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  bool transmission_healty = false;
  
  while (EspSerial.available() > 0 && new_data == false) {
    rc = EspSerial.read();
    if (rc != endMarker) {
      pBuffer[ndx] = rc;
      ndx++;
      if (ndx > uBufSize) {  // index > expected size of the message -->> error
        // transfer corrupted
        transmission_healty = false;
        ndx = uBufSize; 
      }
    }
    else {
      if (ndx < uBufSize ){ // end marker received before reaching the expected size of the message
        transmission_healty = true;
      }
      else{                 // end marker received and message has the expected lenght
        ndx = 0;
        new_data = true;  
        transmission_healty = true;
      } 
    }
    yield();
  }
  return transmission_healty;
}


void HandleNewData() {  
  //Copy it to the temp array because parseData will alter it.
  //strcpy(tempChars, receivedChars);  // original
  int16_t crc;  // calculated crc for the received data
  
  new_data = false;
  memcpy(&temp_dp, pBuffer, uBufSize); // copy pBuffer into temp_dp
  crc = calcCRC(); // calculate crc
  if (crc == temp_dp.crc){
    //memcpy(&dp, pBuffer, uBufSize); // copy pBuffer into dp
    dp = temp_dp;
    missed_packs_count = 0; // reset missed_packs_count
  }
  else {
    missed_packs_tot++;
    missed_packs_count++;
  }
  if (_debug){
    printDp();
  }
}

int16_t calcCRC(){
  int16_t crc;
  String msg; 
  char string[20];
  
  msg = String(temp_dp.FwRev) + String(temp_dp.LeftRight) + String(temp_dp.Pot_P) + String(temp_dp.Pot_I) + String(temp_dp.Pot_D);
  //convert string to char array 
  msg.toCharArray(string, (msg.length()+1)); // +1 for string terminator "\0"
  crc = uCRC16Lib::calculate(string, strlen(string));  
  return crc;
}

void printDp(){   // just for testing
  //Serial.print("Buff Size= "); Serial.print(uBufSize);
  Serial.print("\tFwRev: "); Serial.print(temp_dp.FwRev); 
  Serial.print("\tLeftRight: "); Serial.print(temp_dp.LeftRight); 
  Serial.print("\tP: "); Serial.print(temp_dp.Pot_P);
  Serial.print("\tI: "); Serial.print(temp_dp.Pot_I);
  Serial.print("\tD: "); Serial.print(temp_dp.Pot_D); 
  //Serial.print("\tCRC: "); Serial.println(temp_dp.crc); 
  Serial.println();
  //Serial.print("Missed packs: "); Serial.print(missed_packs_count);
  Serial.print("\ttotal: "); Serial.println(missed_packs_tot);
  Serial.println();
}

void sendDebug(char *message){
  if ((_debug) && (!EspSerial.available())){
    EspSerial.println(message);
  }
}



