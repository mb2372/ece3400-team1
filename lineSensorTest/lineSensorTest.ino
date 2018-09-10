#include <Servo.h>
Servo servoLeft;
Servo servoRight;

void setup() {
  servoLeft.attach(9);
  servoRight.attach(10);
}

void loop() {
  double sensorValue=analogRead(A0);
  Serial.println(sensorValue);
}
