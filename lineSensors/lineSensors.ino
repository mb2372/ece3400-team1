#include <Servo.h>
Servo servoLeft; //left servo 
Servo servoRight; //right servo
double lsensorR; //right line sensor
double lsensorL; //left line sensor
double lsensorM; //middle line sensor

void setup() {
  servoLeft.attach(9);
  servoRight.attach(10);
}

void loop() {
  lsensorR = analogRead(A0);
  lsensorL = analogRead(A0);
  lsensorM = analogRead(A0);
}
