#include "i2cHelpers.h";
#include <Wire.h>

const int Nano_addr=0x08;  // Arduino Nano i2c address
byte data[32];  // receive buffer
int command;
int i2c_wd = 0;  // i2c watchdog counter

void setup_nano_i2c(){
  Wire.begin();        // join i2c bus (address optional for master)

  delay(1000);
}


void write_2_nano() {
  Wire.beginTransmission(8);  // transmit to device #8
  Wire.write(1);              // send command
  Wire.write(3);              // send number of bytes
  Wire.write(i2c_wd);         // sends watchdog counter
  Wire.endTransmission();     // stop transmitting

  // i2c watchdog
  if (i2c_wd < 255) {
    i2c_wd++;   // increment watchdog counter after sending
  }
  else{
    i2c_wd =0 ; // reset to 0
  }
}


void request_from_nano() {
  int i = 0;
  // first send the command
  Wire.beginTransmission(Nano_addr);      // transmit to device #8
  Wire.write(20);                 // send command 20 interpreted as request
  Wire.endTransmission();
  
  Wire.requestFrom(Nano_addr, 8); // request 8 bytes from slave device Arduino Nano
  
  while(Wire.available()){    
    data[i] = Wire.read();
    i++;
  }
  parse_received_data(data);
}


void parse_received_data(byte data[]) {
  
  r_pot1 = data[0];
  r_pot2 = data[1];
  r_pot3 = data[2];
  r_joystick_x = data[3];
  r_joystick_y = data[4];
  r_commands1 = data[5];
  
  r_JSswState       = bitRead(r_commands1, 0);
  r_Sw_On_Off_State = bitRead(r_commands1, 1);
  r_Sw_mode_State   = bitRead(r_commands1, 2);
  r_butt1State      = bitRead(r_commands1, 3);
  r_butt2State      = bitRead(r_commands1, 4);
  r_butt3State      = bitRead(r_commands1, 5);
  r_butt4State      = bitRead(r_commands1, 6);
  
  r_i2c_wd = data[6];      // i2c watchdog counter
  r_chksum = data[7];

  
}

