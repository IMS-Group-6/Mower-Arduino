#include <avr/wdt.h>
#include <MeAuriga.h>
#include "MeEEPROM.h"
#include <Wire.h>
#include <SoftwareSerial.h>


//  Sensor initiation
MeUltrasonicSensor ultraSonic(PORT_10);
MeLineFollower greyScale(PORT_9);
MeGyro gyroMeter(1, 0x69);

//  Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderMotor encoders[2];

//  Variables - Integers
int16_t moveSpeed   = 180;
int16_t turnSpeed   = 180;
int16_t minSpeed    = 45;

void setMotorPwm(int16_t pwm);

void updateSpeed(void);

void setup() {
  Serial.begin(115200);
  gyroMeter.begin();
  

  encoders[0] =  MeEncoderMotor(SLOT1); 
  encoders[1] =  MeEncoderMotor(SLOT2);
  
  encoders->begin();


  wdt_reset();
  encoders->runSpeed(0);

}

void update(void){
  gyroMeter.update();
}

void Forward(void)
{
  Encoder_1.setMotorPwm(-moveSpeed);
  Encoder_2.setMotorPwm(moveSpeed);
}
void Backward(void)
{
  Encoder_1.setMotorPwm(moveSpeed);
  Encoder_2.setMotorPwm(-moveSpeed);
}
void TurnLeft(void)
{
  Encoder_1.setMotorPwm(-moveSpeed);
  Encoder_2.setMotorPwm(moveSpeed/2);
}

void TurnRight(void)
{
  Encoder_1.setMotorPwm(-moveSpeed/2);
  Encoder_2.setMotorPwm(moveSpeed);
}

void StopMotor(void){
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
}


double getGyroX(){
  gyroMeter.update();
  return gyroMeter.getGyroX();
}
double getAngleX(){
  gyroMeter.update();
  return gyroMeter.getAngleX();
}
double getGyroY() {
  gyroMeter.update();
  return gyroMeter.getGyroY();
}

double getAngleY() {
  gyroMeter.update();
  return gyroMeter.getAngleY();
}

double getAngleZ() {
  gyroMeter.update();
  return gyroMeter.getAngleZ();
}

int16_t dist(){
    return ultraSonic.distanceCm();
}
int16_t lineFlag(){
    return greyScale.readSensors();
}

void updateEncoderTick(){
    volatile long leftTick = encoders[0].getCurrentPosition();
    volatile long rightTick = encoders[1].getCurrentPosition();
    Serial.print("Encoder 1 Pos: ");
    Serial.println(leftTick);
    Serial.print("Encoder 2 Pos: ");
    Serial.println(rightTick);
}

void loop() {

  char cmd;
  static bool waitForRaspberry = false;
  
  if(dist()<=15 && !waitForRaspberry){
    StopMotor();
    Serial.println("CAPTURE");  
    waitForRaspberry = true;
    return;
  } 

  // if(lineFlag()<=0){
  //   StopMotor();
  //   return;
  // }

  if (Serial.available() >0){
      cmd = Serial.read();
      Serial.println("Serial available");

      if(waitForRaspberry && cmd == 'c'){
        waitForRaspberry = false;
      }
      
    if(!waitForRaspberry){

    switch(cmd){
      case 'w':
        Forward();
        break;
      case 's':
        Backward();
        break;
      case 'a':
        TurnLeft();
        break;
      case 'd':
        TurnRight();
        break;
      default:
        Serial.write("Unknown command ");
        StopMotor();
        break;
      }
      updateEncoderTick();
    }  
  }
  delay(50);
}
