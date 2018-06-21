#include "Arduino.h"
#include <Wire.h>
//   A4 - I2C SDA 
//   A5 - I2C SCL

const int MPU_addr=0x68;  // MPU6050 address

void mpu_setup(){
  // Set up MPU 6050:
  Wire.begin();
  #if ARDUINO >= 157
    Wire.setClock(400000UL); // Set I2C frequency to 400kHz
  #else
    TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
  #endif
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  delay(2000);
}  // END MPU6050 setup 

void get_angles(double p_ang[]){
  const double degconvert = 57.2957786; //there are like 57 degrees in a radian. 
  double AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; //These will be the raw data from the MPU6050.
  int16_t AcX_calib_val=0.0 , AcY_calib_val=0.0 , AcZ_calib_val=0.0; // accelerometer calibration values
  int16_t GyX_calib_val=0.0 , GyY_calib_val=0.0 , GyZ_calib_val=0.0; // gyro calibration values
  double dt, roll, pitch, gyroXrate, gyroYrate;
   
  //Collect raw data from the sensor.
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  AcX += AcX_calib_val; AcY += AcY_calib_val; AcZ += AcZ_calib_val;
  GyX += GyX_calib_val; GyY += GyY_calib_val; GyZ += GyZ_calib_val;
  
  dt = (double)(micros() - timer) / 1000000; //This line does three things: 1) stops the timer, 2)converts the timer's output to seconds from microseconds, 3)casts the value as a double saved to "dt".
  timer = micros(); //start the timer again so that we can calculate the next dt.

  //the next two lines calculate the orientation of the accelerometer relative to the earth and convert the output of atan2 from radians to degrees
  //We will use this data to correct any cumulative errors in the orientation that the gyroscope develops.
  roll = atan2(AcY, AcZ) * degconvert;
  pitch = atan2(-AcX, AcZ) * degconvert;

  //The gyroscope outputs angular velocities.  To convert these velocities from the raw data to deg/second, divide by 131.  
  //Notice, we're dividing by a double "131.0" 
  gyroXrate = GyX/131.0;
  gyroYrate = GyY/131.0;

  //THE COMPLEMENTARY FILTER
  //This filter calculates the angle based MOSTLY on integrating the angular velocity to an angular displacement.
  //dt, recall, is the time between gathering data from the MPU6050.  We'll pretend that the 
  //angular velocity has remained constant over the time dt, and multiply angular velocity by 
  //time to get displacement.
  //The filter then adds a small correcting factor from the accelerometer ("roll" or "pitch"), so the gyroscope knows which way is down. 
    p_ang[0] = 0.99*(p_ang[0] + gyroXrate * dt) + 0.01*roll; // Calculate the angle using a Complimentary filter
    p_ang[1] = 0.97*(p_ang[1] + gyroYrate * dt) + 0.03*pitch;    // Calculate the angle using a Complimentary filter
}
