#include <Servo.h>
#include <FFT.h> // include the FFT library

//TODO: INCLUDE THE FFT FOR IR AND MICROPHONE. FIX FREE RUNNING MODE VS ANALOGREAD CONFLICT

Servo servoLeft; //left servo 
Servo servoRight; //right servo
double lsensorR; //right line sensor
double lsensorL; //left line sensor
double lsensorM; //middle line sensor
double threshold=900.0; //less than is white, greater than is black
//for the mux with wall sensors as the inputs
int S0 = 7;//pin 7 is S0
int S1 = 6;//pin 6 is S1

//servors
//<90 is clockwise
//>90 is counterclockwise


void setup() {
  servoLeft.attach(9); //left servo
  servoRight.attach(10); //right servo 
  pinMode(LED_BUILTIN,OUTPUT);//testing when we are at intersection or correcting. This can be removed later
  
  //setup for the mux and the wall sensors
  //E AND S2 ARE ALWAYS LOW FOR MUX TO WORK
//      S1     S0      CHANNEL
//      L      L         A0            
//      L       H        A1            
//      H      L         A2       
  pinMode(S0,OUTPUT);//S0
  pinMode(S1,OUTPUT);//S1
  pinMode(A5, INPUT);//output of the mux, input into the Arduino. Need a resistor for this
  Serial.begin(9600);//used for looking at
}

//TODO: METHOD FOR CHECKING MICROPHONE TO KNOW WHEN WE START


//method for wall detecting
void wall_detect(){
  //THIS IS NOT DONE YET. 
  //CURRENTLY READS ALL THREE WALL SENSORS AND STORES INTO OUTPUT VARIABLES
  //TODO: COMPARE THESE WALL DISTANCE VALUES AND MAKE A DECISION ON WHICH DIRECTION TO MOVE AKA WHICH WALL SENSORS ARE HIGH AND LOW AND DO WE GO L,R OR FORWARD
  
  //selecting A0, Low Low
  digitalWrite(S1, LOW);
  digitalWrite(S0, LOW);
  int outA0 = analogRead(A5);
  Serial.println("channel input A0: "+outA0);
  //selecting A1, Low High
  digitalWrite(S1, LOW);
  digitalWrite(S0, HIGH);
  int outA1 = analogRead(A5);
  Serial.println("channel input A1: "+outA1);
  //selecting A2, High Low
  digitalWrite(S1,HIGH);
  digitalWrite(S0,LOW);
  int outA2 = analogRead(A5);
  Serial.println("channel input A2: "+outA2);
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


  //cases for line following and turning
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

  //TODO: CONSTANTLY CHECK IR FOR OTHER ROBOTS

  //intersection detection (back sensors are white)
    if(lsensorR<threshold && lsensorL<threshold && lsensorM<threshold){
      //servoLeft.write(90);
      //servoRight.write(90);
      //delay(10);
      
      //TODO: STOP AND DO WALL DETECTION, THEN DECIDE HOW TO MOVE
      //maybe set the two servos both to 90 to do a brief stop and analyze
      
      forward();
      delay(240);//bump the robot forward a little bit
      //currently defaulted to left turn. Will need to change
      left_turn();
    }
    //not at an intersection: line following and correcting
    else{
      //just here for testing intersection vs not intersection. Can delete later
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
      //just here for testing intersection vs not intersection. Can delete later
      digitalWrite(LED_BUILTIN, LOW);
      
   }

    
  
  
}

