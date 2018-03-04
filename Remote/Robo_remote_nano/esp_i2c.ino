#include <Wire.h>  // Arduino Nano A4=SDA; A5=CLK

// Arduino Nano will join the i2c bus as slave, at address #08
const int Nano_addr=0x08;  // Arduino Nano i2c address (as slave)

byte data[32];
int command;      // i2c command received from master
byte whatisit;
int i2c_wd = 0;  // i2c watchdog counter


typedef struct processData{
  byte pot1;
  byte pot2;
  byte pot3;
  byte joystick_x;
  byte joystick_y;
  byte commands1;
  byte i2c_wd;      // i2c watchdog counter
  byte chksum;
};   // 8 bytes

typedef union I2C_Send{
  processData data;
  byte I2C_packet[sizeof(processData)];
};
I2C_Send send_info;
// END of config data to send to RPi


void setup_nano_i2c(){
  Wire.begin(Nano_addr);        // join i2c bus with address #08
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);

  delay(1);
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
//
void receiveEvent(int howMany){
  int i = 0;
  command = Wire.read(); // first byte is the command (???)
  whatisit = Wire.read();

  while(Wire.available()){    
    data[i] = Wire.read();
    i++;
  }
  
  switch (command) {
    
    case 1:    // read 1 byte (watchdog)
      i2c_wd = data[0];
      
      // i2c watchdog (receive the WD counter from master, increment it)
      if (i2c_wd < 255) {
        i2c_wd++;    // increment watchdog counter after sending
      }
      else{
        i2c_wd = 0 ; // reset to 0
      }

    case 2:    // read bytes
      // parse_receivedData(data);
      break;
  }
 
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  if(command==20){
    Wire.write(send_info.I2C_packet, sizeof(processData));
  }
}


void refresh_I2C_Packet() {
// this function is called in the loop and updates the value that will be sent to master

  send_info.data.pot1 = pot1_val;
  send_info.data.pot2 = pot2_val;
  send_info.data.pot3 = pot3_val;
  send_info.data.joystick_x = JSX_val;
  send_info.data.joystick_y = JSY_val;
  
  //send_info.data.commands1 = 0;
  bitWrite(send_info.data.commands1, 0, JSswState);
  bitWrite(send_info.data.commands1, 1, Sw_On_Off_State);
  bitWrite(send_info.data.commands1, 2, Sw_mode_State);
  bitWrite(send_info.data.commands1, 3, butt1State);
  bitWrite(send_info.data.commands1, 4, butt2State);
  bitWrite(send_info.data.commands1, 5, butt3State);
  bitWrite(send_info.data.commands1, 6, butt4State);
  bitWrite(send_info.data.commands1, 7, 0);
  
  send_info.data.i2c_wd = i2c_wd;
  send_info.data.chksum = 254;
}

void parse_receivedData(byte data[]){
  union float_tag{
    byte b[24];
    float fval[6];
  }ft;

  ft.b[0] = data[0]; ft.b[1] = data[1]; ft.b[2] = data[2]; ft.b[3] = data[3];
  ft.b[4] = data[4]; ft.b[5] = data[5]; ft.b[6] = data[6]; ft.b[7] = data[7];
  ft.b[8] = data[8]; ft.b[9] = data[9]; ft.b[10] = data[10]; ft.b[11] = data[11];
  ft.b[12] = data[12]; ft.b[13] = data[13]; ft.b[14] = data[14]; ft.b[15] = data[15];
  ft.b[16] = data[16]; ft.b[17] = data[17]; ft.b[18] = data[18]; ft.b[19] = data[19];
  ft.b[20] = data[20]; ft.b[21] = data[21]; ft.b[22] = data[22]; ft.b[23] = data[23];

//  rem_angleX = ft.fval[0];
//  rem_angleY = ft.fval[1];
//  rem_PID_Kp = ft.fval[2];
//  rem_PID_Ki = ft.fval[3];
//  rem_PID_Kd = ft.fval[4];
//  rem_PID_setpoint = ft.fval[5];
//  rem_PID_output = ft.fval[6];

}  

