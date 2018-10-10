#include <Servo.h>
Servo servoLeft; //left servo 
Servo servoRight; //right servo
double lsensorR; //right line sensor
double lsensorL; //left line sensor
double lsensorM; //middle line sensor
double threshold=900.0; //less than is white, greater than is black
//servors
//<90 is clockwise
//>90 is counterclockwise


void setup() {
  servoLeft.attach(9); //left servo
  servoRight.attach(10); //right servo 
  pinMode(LED_BUILTIN,OUTPUT);
}
//a turn 90 to the right
void right_turn(){
  servoLeft.write(180);
  servoRight.write(180);
  delay(670);
}
//a turn 90 degrees to the left
void left_turn(){
  servoLeft.write(0);
  servoRight.write(0);
  delay(670);
}
//correction, not a turn
void slight_right(){
    servoRight.write(95);
    servoLeft.write(98);
    delay(100);
}
//correction, not a turn
void hard_right(){
   servoRight.write(95);
   servoLeft.write(102);
   delay(100);
}
//correction, not a turn
void slight_left(){
  servoRight.write(83);
  servoLeft.write(85);
  delay(100);
}
//correction, not a turn
void hard_left(){
  servoRight.write(79);
  servoLeft.write(85);
  delay(100);
}

void forward(){
  servoRight.write(0);
  servoLeft.write(180);  
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
    if(lsensorR<threshold && lsensorL<threshold && lsensorM<threshold){
      //servoLeft.write(90);
      //servoRight.write(90);
      //delay(10);
      forward();
      delay(240);
      left_turn();
    }

    else{
      digitalWrite(LED_BUILTIN, HIGH);
      
      //hard right correction: right is white, left and middle are black
      if(lsensorR<threshold && lsensorL>=threshold && lsensorM>=threshold){
        hard_right();
      }
    //slight right correction: right is white, left is black, middle is white
      else if(lsensorR<threshold && lsensorL>=threshold && lsensorM<threshold){
          slight_right();
      }
    //left correction: left is white and middle and right are black
      else if(lsensorR>=threshold && lsensorL<threshold && lsensorM>=threshold){
        hard_left();
      }
     //slight left correction: left and middle are white and right is black
      else if(lsensorR>=threshold && lsensorL<threshold && lsensorM<threshold){
        slight_left();
      }
        //move forward:right is black, left is black
      else if(lsensorR>=threshold && lsensorL>=threshold && lsensorM<threshold){
        forward();
      }
      digitalWrite(LED_BUILTIN, LOW);
      
   }

    
  
  
}

