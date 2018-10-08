#include <Servo.h>
Servo servoLeft; //left servo 
Servo servoRight; //right servo
double lsensorR; //right line sensor
double lsensorL; //left line sensor
double lsensorM; //middle line sensor
double threshold=300.0; //less than is white, greater than is black

void setup() {
  servoLeft.attach(9); //left servo
  servoRight.attach(10); //right servo 
}

void slight_right(){
  
}

void hard_right(){
  
}

void slight_left(){
  
}

void hard_left(){
  
}



void loop() {
  lsensorR = analogRead(A0);
  lsensorL = analogRead(A1);
  lsensorM = analogRead(A2);

  //cases
  /*
  LMR
  BBB: go forward left and right are black
  BWB: go forward left and right are black
  WWW: intersection left and right are white
  WBW: intersection left and right are white
  BBW: hard right left and middle are black and right is white
  BWW: slight right left is black and middle and right are white
  WBB: hard left left is white, middle and right are white
  WWB: slight left  
  */

  //intersection detection (back sensors are white)
    if(lsensorR<threshold && lsensorL<threshold){
      servoRight.write(150);
      servoLeft.write(170);
      delay(50);
    }
  
  //move forward:right is black, left is black
    if(lsensorR>=threshold && lsensorL>=threshold){
    servoRight.write(0);
    servoLeft.write(180);
    }
  
  //hard right correction: right is white, left and middle are black
    if(lsensorR<threshold && lsensorL>=threshold && lsensorM>=threshold){
    servoRight.write(95);
    servoLeft.write(102);
    delay(100);
    }
  //slight right correction: right is white, left is black, middle is white
    if(lsensorR<threshold && lsensorL>=threshold && lsensorM<threshold){
    servoRight.write(95);
    servoLeft.write(98);
    delay(100);
    }
  //left correction: left is white and middle and right are black
    if(lsensorR>=threshold && lsensorL<threshold && lsensorM>=threshold){
    servoRight.write(79);
    servoLeft.write(85);
    delay(100);
    }
   //slight left correction: left and middle are white and right is black
    if(lsensorR>=threshold && lsensorL<threshold && lsensorM<threshold){
    servoRight.write(83);
    servoLeft.write(85);
    delay(100);
    }
  
}

