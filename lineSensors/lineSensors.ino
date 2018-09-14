#include <Servo.h>
Servo servoLeft; //left servo 
Servo servoRight; //right servo
double lsensorR; //right line sensor
double lsensorL; //left line sensor
double lsensorM; //middle line sensor
double threshold=900.0; 
int counter=0;

void setup() {
  Serial.begin(9600);
  servoLeft.attach(9); //left servo
  servoRight.attach(10); //right servo 
  /*
  Serial.begin(9600);
  servoRight.write(90);
  servoLeft.write(90);
  */
}
void loop() {
  lsensorR = analogRead(A0);
  lsensorL = analogRead(A1);
  lsensorM = analogRead(A2);
  counter=(millis()/1000)*.4;
  /*
  Serial.print("Left Sensor: ");
  Serial.println(lsensorL);
  Serial.print("Right Sensor: ");
  Serial.println(lsensorR);
  Serial.print("Middle Sensor: ");
  Serial.println(lsensorM);
  Serial.print("-----------------------");
  delay(3000);
  */
  Serial.println(counter);
  //move forward
    if(lsensorR>=threshold && lsensorL>=threshold && lsensorM<threshold){
    servoRight.write(0);
    servoLeft.write(180);
    }
  //right turn 
    if(lsensorR<threshold && lsensorL>=threshold && lsensorM>=threshold){
    servoRight.write(95);
    servoLeft.write(98);
    delay(10);
    }
  //left turn 
    if(lsensorR>=threshold && lsensorL<threshold && lsensorM>=threshold){
    servoRight.write(85);
    servoLeft.write(88);
    delay(10);
    }
  //all sensors are on black 
    if(lsensorR>=threshold && lsensorL>=threshold && lsensorM>=threshold){
    servoRight.write(0);
    servoLeft.write(180);
    }
  //intersection detection (all sensors are white)
    if(lsensorR<threshold && lsensorL<threshold && lsensorM<threshold){
      if(counter%8<=3){
      delay(50);
      servoRight.write(95);
      servoLeft.write(110);
      
        }
      else if(counter%8>3){
      delay(50);
      servoRight.write(70);
      servoLeft.write(85);
      }
}
}
