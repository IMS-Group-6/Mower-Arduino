#include <avr/wdt.h>
#include <MeAuriga.h>
#include "MeEEPROM.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#include <math.h>


//  Sensor initiation
MeUltrasonicSensor ultraSonic(PORT_10);
MeLineFollower greyScale(PORT_9);
MeGyro gyroMeter(1, 0x69);

//  Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderMotor encoders[2];

//  Variables - Integers
<<<<<<< Updated upstream
int16_t moveSpeed   = 180;
int16_t turnSpeed   = 180;
int16_t minSpeed    = 45;

=======
int16_t moveSpeed   = 140;
int16_t turnSpeed   = 140;
int16_t minSpeed    = 45;

// Distance traveled
int travForward = 0;
int travBackwards = 0;
int travRight = 0;
int travLeft = 0;


>>>>>>> Stashed changes
void setMotorPwm(int16_t pwm);

void updateSpeed(void);

void setup() {
  Serial.begin(115200);
  gyroMeter.begin();
  

<<<<<<< Updated upstream
  encoders[0] =  MeEncoderMotor(SLOT1); 
  encoders[1] =  MeEncoderMotor(SLOT2);
  
  encoders->begin();
=======
  encoders[0] = MeEncoderMotor(SLOT1);
  encoders[1] = MeEncoderMotor(SLOT2);

  encoders[0].begin();
  encoders[1].begin();

>>>>>>> Stashed changes


  wdt_reset();
  encoders[0].runSpeed(0);
  encoders[1].runSpeed(1);

  travForward = 0;
  travBackwards = 0;
  travRight = 0;
  travLeft = 0;

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

<<<<<<< Updated upstream
void updateEncoderTick(){
    volatile long leftTick = encoders[0].getCurrentPosition();
    volatile long rightTick = encoders[1].getCurrentPosition();
    Serial.print("Encoder 1 Pos: ");
    Serial.println(leftTick);
    Serial.print("Encoder 2 Pos: ");
    Serial.println(rightTick);
}

=======

void checkPwm(MeEncoderOnBoard &motor1, MeEncoderOnBoard &motor2){
  
  // Moving forward
  if((motor1.getCurPwm() == -moveSpeed) && (motor2.getCurPwm() == moveSpeed)){
      travForward++;
      return;
  }

  // Moving backwards
  if((motor1.getCurPwm() == moveSpeed) && (motor2.getCurPwm() == -moveSpeed)){
      travBackwards++;
      return;
  }

  // Moving right
  if((motor1.getCurPwm() == -moveSpeed/2) && (motor2.getCurPwm() == moveSpeed)){
      travRight++;
      return;
  }

  // Moving left
  if((motor1.getCurPwm() == -moveSpeed) && (motor2.getCurPwm() == moveSpeed/2)){
      travLeft++;
      return;
  }
}


>>>>>>> Stashed changes
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
  checkPwm(Encoder_1, Encoder_2);
  Serial.println("travForward: " + String(travForward));
  Serial.println("travBackwards: " + String(travBackwards));
  Serial.println("travLeft: " + String(travLeft));
  Serial.println("travRight: " + String(travRight));
  delay(50);
}
