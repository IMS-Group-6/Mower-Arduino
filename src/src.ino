#include <avr/wdt.h>
#include <MeAuriga.h>
#include "MeEEPROM.h"
#include <Wire.h>
#include <SoftwareSerial.h>

// Global States
enum Mode {MANUAL_MODE, AUTONOMOUS_MODE};
enum MowerState { IDLE, 
                  FORWARD, 
                  BACKWARD, 
                  RIGHT, 
                  LEFT, 
                  OBSTACLE, 
                  BORDER,
                  STOP, 
                  AVOID_OBSTACLE, 
                  AVOID_BORDER 
                  };

// Sensor initiation
MeUltrasonicSensor ultraSonic(PORT_10);
MeLineFollower greyScale(PORT_9);

// Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderMotor encoders[2];

// Variables - Integers
int16_t moveSpeed   = 110;
int16_t turnSpeed   = 110;
int16_t minSpeed    = 45;


void setMotorPwm(int16_t pwm);

void updateSpeed(void);

// Setup inturrupts for encoders on both motors
// Invert one of them as this so both values when positive means forward
void interrupt_encoder1(void)
{
  if (digitalRead(Encoder_1.getPortB()) != 0)
  {
    Encoder_1.pulsePosMinus();
  }
  else
  {
    Encoder_1.pulsePosPlus();
  }
}

void interrupt_encoder2(void)
{
  if (digitalRead(Encoder_2.getPortB()) == 0)
  {
    Encoder_2.pulsePosMinus();
  }
  else
  {
    Encoder_2.pulsePosPlus();
  }
}

void setup()
{
  Serial.begin(115200);
  

  encoders[0] = MeEncoderMotor(SLOT1);
  encoders[1] = MeEncoderMotor(SLOT2);

  encoders[0].begin();
  encoders[1].begin();

  attachInterrupt(Encoder_1.getIntNum(), interrupt_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), interrupt_encoder2, RISING);

  wdt_reset();
  encoders[0].runSpeed(0);
  encoders[1].runSpeed(1);

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
  Encoder_2.setMotorPwm(-moveSpeed);
}

void TurnRight(void)
{
  Encoder_1.setMotorPwm(moveSpeed);
  Encoder_2.setMotorPwm(moveSpeed);
}

void StopMotor(void)
{
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
}


void reportOdometry() 
{
  Encoder_1.updateCurPos();
  Encoder_2.updateCurPos();

  char charValA[20];
  sprintf(charValA, ", %08d", Encoder_1.getCurPos());
  char charValB[20];
  sprintf(charValB, ", %08d", Encoder_2.getCurPos());

  Serial.print("ENCODER, ");
  Serial.print(charValA);
  Serial.print(",");
  Serial.println(charValB);
}

int16_t distanceToObject(){ return ultraSonic.distanceCm();}
int16_t borderDetector(){ return greyScale.readSensors();}


Mode currentMode = AUTONOMOUS_MODE;
MowerState mowerState = IDLE;


void loop()
{

  char cmd;

  if (Serial.available()>0){
      cmd = Serial.read();
      Serial.println("Serial available");
       // Handle these commands regardless of the mode.
      if (cmd == 'x'){
        mowerState = STOP;
        currentMode = AUTONOMOUS_MODE;
        StopMotor();
      }
      else if (cmd == 'm'){
        currentMode = MANUAL_MODE;
        mowerState = IDLE;
        StopMotor();
      }
      else if (cmd == 't'){
        Serial.println("Switch to Autonomous mode");
        currentMode = AUTONOMOUS_MODE;
        mowerState = IDLE;
        StopMotor();
      }

      // Handle these commands only in manual mode.
      else if (currentMode==MANUAL_MODE){
        switch (cmd)
        {
        case 'w':
          Forward();
          break;
        case 's':
          Backward();
          break;
        case 'd':
          TurnRight();
          break;
        case 'a':
          TurnLeft();
          break;
        default:
          // Serial.write("Unknown Command ");
          // StopMotor();
          break;
        }
      }
  }
  
    if(currentMode == AUTONOMOUS_MODE){
      if(cmd == 'x'){
        mowerState = STOP;
        StopMotor();
        return;
      }
      if(cmd == 'm'){
        currentMode = MANUAL_MODE;
        mowerState = IDLE;
        StopMotor();
        return;
      }
      if(cmd == 'z'){
        mowerState = IDLE;
        return;
      }

      switch(mowerState) {
      case IDLE:
        if(cmd == 'z'){
          Serial.println("Is in IDLE, Will change to FORWARD");
          mowerState = FORWARD;
        }
        break;
      case FORWARD:
        Forward();
        if(distanceToObject()<= 13){
          Serial.println("CAPTURE");
          StopMotor();
          mowerState = OBSTACLE;
        }
        if(borderDetector() <= 0){
          StopMotor();
          Serial.println("BORDER");
          mowerState = BORDER;
        }
        break;
      case OBSTACLE:
        Backward();
        if(distanceToObject() > 20){
          StopMotor();
          mowerState = AVOID_OBSTACLE;
        }
        break;
      case BORDER:
        Backward();
        if(borderDetector() > 0){
          StopMotor();
          mowerState = AVOID_BORDER;
        }
        break;
      case AVOID_OBSTACLE:
        TurnLeft();
        if(distanceToObject() > 25){
          StopMotor();
          mowerState = FORWARD;
        }
      case AVOID_BORDER:
        TurnLeft();
        delay(700);
        mowerState = FORWARD;
        break;
    }
  } 
 //reportOdometry();
}