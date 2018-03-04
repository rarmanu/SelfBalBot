/* Self balancing robot
  Arduino Nano code for remote control
*/
#include "Arduino.h"
#include <Wire.h>
#include "IO.h";


void setup() {
  Serial.begin(9600);
  setup_IOs();

  
  setup_nano_i2c();
}




float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup_IOs(){
  //configure digital inputs for pushbuttons and enable the internal pull-up resistors
  //connected between pin and GND (state is HIGH for button not pressed)
  pinMode(JSsw, INPUT_PULLUP);  // joystick switch
  pinMode(butt1, INPUT_PULLUP);
  pinMode(butt2, INPUT_PULLUP);
  pinMode(butt3, INPUT_PULLUP);
  pinMode(butt4, INPUT_PULLUP);
  //configure digital inputs for on/off switches
  //connected between pin and 5v (pulldown resistor of 10k needed)
  pinMode(Sw_On_Off, INPUT);
  pinMode(Sw_mode, INPUT);
  
  //configure PWM pins and DO pins
  pinMode(PWM1_pin, OUTPUT);
  pinMode(PWM2_pin, OUTPUT);
  pinMode(PWM3_pin, OUTPUT);
  pinMode(Run_led_pin, OUTPUT);
  pinMode(i2c_wd_led_pin, OUTPUT);
}

void read_inputs(){
  // read AIs and map the values in [0-255] interval
  pot1_val = map(analogRead(pot1), 0, 1023, 0, 255);
  pot2_val = map(analogRead(pot2), 0, 1023, 0, 255);
  pot3_val = map(analogRead(pot3), 0, 1023, 0, 255);
  JSX_val  = map(analogRead(JSX), 0, 1023, 0, 255);
  JSY_val  = map(analogRead(JSY), 0, 1023, 0, 255);
  
  JSswState = digitalRead(JSsw);
}

void write_outputs(){
  
  analogWrite(PWM1_pin, PWM1_out);
  analogWrite(PWM2_pin, PWM2_out);
  analogWrite(PWM3_pin, PWM3_out);

  digitalWrite(Run_led_pin, LOW);
  digitalWrite(i2c_wd_led_pin, LOW);
  
}


void loop() {
  read_inputs();

  write_outputs();
}
