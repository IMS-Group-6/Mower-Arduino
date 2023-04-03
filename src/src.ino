#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeAuriga.h>

MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeUltrasonicSensor ultrasonicSensor(PORT_3);

SoftwareSerial RaspberryPiSerial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  // RaspberryPiSerial.begin(9600);
}

void controlMotors(int left, int right) {
  Encoder_1.setMotorPwm(left);
  Encoder_2.setMotorPwm(right);
}

void sendUltrasonicData() {
  int distance = ultrasonicSensor.distanceCm();
  Serial.print("U:");
  Serial.println(distance);
}

void loop() {
  if(Serial.available()){
    String cmd = Serial.readStringUntil('\n');

    if(cmd=="w"){
      controlMotors(-180, 180);
    }
    else if(cmd=="w"){
      controlMotors(180, -180);
    }
    else if(cmd=="a"){
      controlMotors(-180, 0);
    }
    else if(cmd=="d"){
      controlMotors(0, 180);
    }
    else if(cmd=="stop"){
      controlMotors(0,0);
    }
  }
  delay(200);
}
