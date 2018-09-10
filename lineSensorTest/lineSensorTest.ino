#include <Servo.h>
Servo servoLeft; //left servo 
Servo servoRight; //right servo

void setup() {
  servoLeft.attach(9); //left servo
  servoRight.attach(10); //right servo 
}
void loop() {
  servoLeft.write(87);
  servoRight.write(87);
}
