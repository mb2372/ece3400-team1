#include <Servo.h>
#include <FFT.h> // include the FFT library

//TODO: INCLUDE THE FFT FOR IR AND MICROPHONE. FIX FREE RUNNING MODE VS ANALOGREAD CONFLICT


//declaring servors
//<90 is clockwise
//>90 is counterclockwise
Servo servoLeft; //left servo 
Servo servoRight; //right servo

//declaring line sensors & black/white threshold
double lsensorR; //right line sensor
double lsensorL; //left line sensor
double lsensorM; //middle line sensor
double line_threshold=900.0; //less than is white, greater than is black

//declaring wall sensors & wall/no wall threshold
double l_wall_sensor; //left wall sensor
double r_wall_sensor; //right wall sensor
double f_wall_sensor; //front wall sensor
double wall_threshold=150.0;//less is no wall, > is wall

//initializing select pins for the mux with wall sensors
int S0 = 7;//pin 7 is S0
int S1 = 6;//pin 6 is S1


void setup() {
  servoLeft.attach(9); //left servo
  servoRight.attach(10); //right servo 
  pinMode(LED_BUILTIN,OUTPUT);//testing when we are at intersection or correcting. This can be removed later  
  pinMode(S0,OUTPUT);//input select signal 0 for mux
  pinMode(S1,OUTPUT);//input select signal 1 for mux
  pinMode(A5, INPUT);//output of the mux, input into the Arduino. Need a resistor for this
  Serial.begin(9600);//for testing the wall output, can delete later
}

//TODO: METHOD FOR CHECKING MICROPHONE TO KNOW WHEN WE START


//METHODS FOR WALL DETECTION
//////////////////////////////////////////
//setup for the mux and the wall sensors
//E AND S2 ARE ALWAYS LOW FOR MUX TO WORK
//      S1     S0      WALL SENSOR
//      L      L          LEFT            
//      L      H          RIGHT            
//      H      L          FRONT  
//////////////////////////////////////////

void left_wall_detect(){
  //setting mux select signals
  digitalWrite(S1, LOW);
  digitalWrite(S0, LOW);
  l_wall_sensor = analogRead(A5);
}
void right_wall_detect(){
  //setting mux select signals
  digitalWrite(S1, LOW);
  digitalWrite(S0, HIGH);
  r_wall_sensor = analogRead(A5);
}
void front_wall_detect(){
  //setting mux select signals
  digitalWrite(S1,HIGH);
  digitalWrite(S0,LOW);
  f_wall_sensor = analogRead(A5);
}

//METHODS FOR TURNING/MOVING FORWARD/STOPPING
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
//move forward
void forward(){
  servoRight.write(0);
  servoLeft.write(180);  
}
//stop
void pause(){
  servoRight.write(90);
  servoLeft.write(90);  
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
    if(lsensorR<line_threshold && lsensorL<line_threshold && lsensorM<line_threshold){
      //TODO: STOP AND DO WALL DETECTION, THEN DECIDE HOW TO MOVE
      //maybe set the two servos both to 90 to do a brief stop and analyze
      forward();
      delay(240);//bump the robot forward a little bit
      pause();//stops the robot from moving
      //maybe have delay here??
      front_wall_detect();
      right_wall_detect();
      left_wall_detect();
      delay(500);
      //no wall in the front and no wall on right or left
      //can we just have a if front is open go forward??
      if(f_wall_sensor<wall_threshold){
        forward();  
      }
      
   /*   if(l_wall_sensor>=wall_threshold && r_wall_sensor>=wall_threshold && f_wall_sensor<wall_threshold){
        forward();
      }*/
      //no wall on the right and walls on left and front
      if(l_wall_sensor>=wall_threshold && r_wall_sensor<wall_threshold && f_wall_sensor>=wall_threshold){
        right_turn();
      }
      //no wall on the left and walls on right and front
      if(l_wall_sensor<wall_threshold && r_wall_sensor>=wall_threshold && f_wall_sensor>=wall_threshold){
        left_turn();
      }
      //walls everywhere. Uturn?
      if(l_wall_sensor>=wall_threshold && r_wall_sensor>=wall_threshold && f_wall_sensor>=wall_threshold){
        left_turn();
        left_turn();
      }
      
     }
    //not at an intersection: line following and correcting
    else{
      //just here for testing intersection vs not intersection. Can delete later
      digitalWrite(LED_BUILTIN, HIGH);
      
      //hard right correction: right is white, left and middle are black
      if(lsensorR<line_threshold && lsensorL>=line_threshold && lsensorM>=line_threshold){
        hard_right();
      }
    //slight right correction: right is white, left is black, middle is white
      else if(lsensorR<line_threshold && lsensorL>=line_threshold && lsensorM<line_threshold){
          slight_right();
      }
    //left correction: left is white and middle and right are black
      else if(lsensorR>=line_threshold && lsensorL<line_threshold && lsensorM>=line_threshold){
        hard_left();
      }
     //slight left correction: left and middle are white and right is black
      else if(lsensorR>=line_threshold && lsensorL<line_threshold && lsensorM<line_threshold){
        slight_left();
      }
        //move forward:right is black, left is black
      else if(lsensorR>=line_threshold && lsensorL>=line_threshold && lsensorM<line_threshold){
        forward();
      }
      //just here for testing intersection vs not intersection. Can delete later
      digitalWrite(LED_BUILTIN, LOW);
   }
}

