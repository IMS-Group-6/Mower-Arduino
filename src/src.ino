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
int16_t moveSpeed   = 160;
int16_t turnSpeed   = 160;
int16_t minSpeed    = 45;

// Variables - Double

// RGB Matrix
MeRGBLed led_ring(0,12);


void setMotorPwm(int16_t pwm);

void updateSpeed(void);

void setup() {
  Serial.begin(115200);
  gyroMeter.begin();
  

  encoders[0], encoders[1] = MeEncoderMotor(SLOT1), MeEncoderMotor(SLOT2);
  encoders->begin();

  wdt_reset();
  encoders->runSpeed(0);

  /* //Set Pwm 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);

  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  while (!Serial) {
    ; // wait for serial port to connect via USB
  } */
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


void loop() {

  char cmd;


  if(dist()<=15){
    StopMotor();
    return;
  } 

  if(lineFlag()<=0){
    StopMotor();
    return;
  }

  if (Serial.available() >0){
      cmd = Serial.read();
      Serial.println("Serial available");
    
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
        Serial.println();
        StopMotor();
        break;
    }  
  }
  delay(50);
}
