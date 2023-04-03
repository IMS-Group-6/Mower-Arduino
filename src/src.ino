#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeMCore.h>

MeDCMotor motorLeft(M1);
MeDCMotor motorRight(M2);
MeUltrasonicSensor ultrasonicSensor(PORT_3);

SoftwareSerial RaspberryPiSerial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  RaspberryPiSerial.begin(9600);
}

void controlMotors(int leftSpeed, int rightSpeed) {
  motorLeft.run(leftSpeed);
  motorRight.run(rightSpeed);
}

void sendUltrasonicData() {
  int distance = ultrasonicSensor.distanceCm();
  RaspberryPiSerial.print("U:");
  RaspberryPiSerial.println(distance);
}

void loop() {
  if (RaspberryPiSerial.available()) {
    String command = RaspberryPiSerial.readStringUntil('\n');

    if (command == "forward") {
      controlMotors(100, 100);
    } else if (command == "backward") {
      controlMotors(-100, -100);
    } else if (command == "left") {
      controlMotors(-100, 100);
    } else if (command == "right") {
      controlMotors(100, -100);
    } else if (command == "stop") {
      controlMotors(0, 0);
    }
  }

  sendUltrasonicData();
  delay(100);
}
