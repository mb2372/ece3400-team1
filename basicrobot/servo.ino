#include <Servo.h>
Servo servo1; //right
Servo servo2; //left

void setup() {
  // put your setup code here, to run once:
  servo1.attach(9);
  servo2.attach(10);
  
}
int duration=1200;
void loop() {
  //move forward for 2s
  servo1.write(0);//clockwise
  servo2.write(180);//counterclockwise
  delay(3000);

  //stop for 1s
  servo1.write(90);
  servo2.write(90);
  delay(1000);

  //move back for 2s
  servo1.write(180);
  servo2.write(0);
  delay(2000);

  //stop for 1s
  servo1.write(90);
  servo2.write(90);
  delay(1000);

  //rotate clockwise for duration
  servo1.write(90);
  servo2.write(180);
  delay(duration);

  //stop for 1s
  servo1.write(90);
  servo2.write(90);
  delay(1000);

  //rotate counter-clockwise for duration 
  servo2.write(90);
  servo1.write(0);
  delay(duration);
}
