// define inputs and outputs for the remote

#ifndef IO_H
#define IO_H
  const int pot1 = A0;
  const int pot2 = A1;
  const int pot3 = A2;
  // A4 = i2c SDA
  // A5 = i2c SLC
  const int JSX = A6;       // joystick X
  const int JSY = A7;       // joystick Y
  
  const int JSsw = 2;       // DI
  const int Sw_On_Off = 3;  // DI
  const int Sw_mode = 4;    // DI (mode: 0=Joystick ; 1=MPU6050)
  const int butt1 = 5;      // DI
  const int butt2 = 6;      // DI
  const int butt3 = 7;      // DI
  const int butt4 = 8;      // DI
  
  const int PWM1_pin = 9;     // DO PWM
  const int PWM2_pin = 10;    // DO PWM
  const int PWM3_pin = 11;    // DO PWM
  const int Run_led_pin = 12; // DO
  const int i2c_wd_led_pin = 13;  // DO i2c watchdog

  // values read from the pots
  int pot1_val = 0;
  int pot2_val = 0;
  int pot3_val = 0;
  int JSX_val = 0;
  int JSY_val = 0;

  int JSswState = 0;
  int Sw_On_Off_State = 0;
  int Sw_mode_State = 0;
  int butt1State = 0;
  int butt2State = 0;
  int butt3State = 0;
  int butt4State = 0;
  
  int PWM1_out = 0;
  int PWM2_out = 0;
  int PWM3_out = 0;
  
#endif
