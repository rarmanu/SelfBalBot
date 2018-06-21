/*  Working so far:
 *  - software serial communication working between arduino(sender) and ESP8266(receiver)  
 *  - 
 *  -
 * TO DO: 
 *  - add MPU
 *  - operating mode selection code
 *  - 
 */

#include <uCRC16Lib.h>  // library for CRC calc
#include <Wire.h>
//   A4 - I2C SDA 
//   A5 - I2C SCL
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3);   // RX, TX
// Pin assignment
// AI:
const int XPin      = A0;        // x direction   
const int YPin      = A1;        // y direction   
const int PidP_Pin  = A2;        // PID P param pot
const int PidI_Pin  = A3;        // PID I param pot
const int PidD_Pin  = A6;        // PID D param pot
// DI:
const int pushButton = 4;    // joystick pushbutton
// DO:
const int dirLedPins[] = { 8, 9, 10, 11 };  // an array of pin numbers to which LEDs are attached
const int ModeLedPin = 12;  // LED used for signaling the current operating mode (MPU / Switch)
const int ledPin = 13;      // onboard LED
// <| end of pin assignment 

// variables to store digital input  |>
int ButPressed = false; 
// <| end 

// Rutine for repeat part of the code every X miliseconds
#define runEvery(t) for (static long _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))
// derived signals  |>
const int ledPinCount = 4;  // number of LEDs
int _debug = false;         // enable/disable Serial.print
uint32_t timer;
bool ctrlMode = 0;          //  0 = joystick ; 1 = MPU6050
bool ctrlModeChanged = 0;   // Control mode changed flag
int16_t count = 0;          // test/debug
// <| end 

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
// <| end


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (_debug) {
    Serial.println("OK!");
  }
  // set the data rate for the SoftwareSerial port
  EspSerial.begin(9600);
  if (_debug) {
    Serial.println("ESP Serial OK!");
  }
  //EspSerial.println("Hello, ESP8266!!");

  setup_board();
  init_board();
  init_dp();
  timer = micros(); //start timer
}

void loop() {
  // check button state |>
  // Detection of mode change request (<MPU / Joystick)  |>
  ButPressed = digitalRead(pushButton);
  if ( ButPressed && !ctrlModeChanged ){
    ctrlMode = !ctrlMode;   // switch between control modes (joystick / MPU sensor)
    ctrlModeChanged = 1;  
  }
  if (!ButPressed && ctrlModeChanged){
    ctrlModeChanged = 0;  // reset flag
  }
  // LED will be ON only if MPU is used
  if (ctrlMode) {
    digitalWrite(ModeLedPin, HIGH);
  }
  // <| end
  
  runEvery(10) {
    update_dp();  // read analog inputs
    serialSend();
    //sendStructure((char *) &dp, sizeof(data_packet_t));
  }
}

void setup_board(){
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
}

void init_board(){
  /*
   * initialize boar - run only at sturt-up
   * (will light the LEDs)
   */
  // loop from the lowest pin to the highest:
  for (int thisPin = 0; thisPin < ledPinCount; thisPin++) {
    // turn the pin on:
    digitalWrite(dirLedPins[thisPin], HIGH);
    delay(150);
    // turn the pin off:
    digitalWrite(dirLedPins[thisPin], LOW);

  }
  // loop from the highest pin to the lowest:
  for (int thisPin = ledPinCount - 1; thisPin >= 0; thisPin--) {
    // turn the pin on:
    digitalWrite(dirLedPins[thisPin], HIGH);
    delay(150);
    // turn the pin off:
    digitalWrite(dirLedPins[thisPin], LOW);
  }
}

void init_dp(){
  /*
   * Init data packet
   */
  dp.FwRev      = 0;  dp.LeftRight  = 0;
  dp.Pot_P      = 0;  dp.Pot_I      = 0;
  dp.Pot_D      = 0;  dp.crc        = 0;
}

void update_dp(){
  /*
   * Update data pack before sending
   * Only raw values will get sent over serial (values to be processed by receiver)
   */ 
  if (!ctrlMode){		// in joystick control mode
  /* send joystick reading (values between 0 - 1023) */
	dp.FwRev      = (int16_t)analogRead(XPin);
	dp.LeftRight  = (int16_t)analogRead(YPin);
  }
  else {	// use MPU readings
	get_angles(angles);
  }
  
  dp.Pot_P      = (int16_t)analogRead(PidP_Pin);
  dp.Pot_I      = (int16_t)analogRead(PidI_Pin); 
  dp.Pot_D      = (int16_t)analogRead(PidD_Pin);
  dp.crc        = calcCRC();

  if (_debug) {
    Serial.print("Size of struct: "); Serial.print(sizeof(data_packet_t)); 
    Serial.print("\tFwRev: ");        Serial.print(dp.FwRev); 
    Serial.print("\tLeftRight: ");    Serial.print(dp.LeftRight); 
    Serial.print("\tP: ");            Serial.print(dp.Pot_P);    
    Serial.print("\tI: ");            Serial.print(dp.Pot_I);
    Serial.print("\tD: ");            Serial.print(dp.Pot_D);
    Serial.print("\tCRC: ");          Serial.println(dp.crc); 
  }
}

void serialSend() {
  unsigned long uBufSize = sizeof(data_packet_t);
  char pBuffer[uBufSize];
 
  memcpy(pBuffer, &dp, uBufSize);
  for(int i = 0; i<uBufSize; i++) {
    EspSerial.write(pBuffer[i]);
    //EspSerial.print(pBuffer[i]);
  }
  EspSerial.println();  // new line (end) marker
}

int16_t calcCRC(){
  int16_t crc;
  String msg; 
  char string[20];
  
  msg = String(dp.FwRev) + String(dp.LeftRight) + String(dp.Pot_P) + String(dp.Pot_I) + String(dp.Pot_D);
  //convert string to char array 
  msg.toCharArray(string, (msg.length()+1)); // +1 for string terminator "\0"
  /* Serial.print(string); Serial.print(" "); Serial.println(strlen(string)); */
  crc = uCRC16Lib::calculate(string, strlen(string));  
  return crc;
}
  
