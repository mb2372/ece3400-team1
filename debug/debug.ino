#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the FFT library
#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <StackArray.h>
//DFS data structures------------------------------------------------------------
StackArray<int> stack;
int row = 8;
int col = 0;
boolean visited[9][9] = {
  {1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}, 
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}, 
};

//-------------------------------------------------------------------------
//SERVOS
//<90 is clockwise
//>90 is counterclockwise
Servo servoLeft; //left servo 
Servo servoRight; //right servo

//LINE SENSORS------------------------------------------------------------------------
//declaring line sensors & black/white threshold
int lsensorR; //right line sensor
int lsensorL; //left line sensor
int lsensorM; //middle line sensor
int line_threshold=925; //less than is white, greater than is black

//WALLS-------------------------------------------------------------------------------------
//declaring wall sensors & wall/no wall threshold
int l_wall_sensor; //left wall sensor
int r_wall_sensor; //right wall sensor
int f_wall_sensor; //front wall sensor
int f_wall_threshold=130;//less is no wall, > is wall
int l_wall_threshold = 220;
int r_wall_threshold = 150;

//average array 
int avg_reading=0;

//initializing select pins for the mux with wall sensors
int S0 = 7;//pin 7 is S0
int S1 = 6;//pin 6 is S1

//LED PINS-------------------------------------------------------------------------------
// led pins
int yellow_led = 2;
int green_led = 1; 

//RADIO------------------------------------------------------------------------------------------
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9, 10);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x0000000002LL, 0x0000000003LL };

//00 is north = 0
int north = 0;
//01 is east = 1
int east = 1;
//10 is south = 2
int south =2;
//11 is west = 3
int west = 3;
int dir = south; 
int rightWallSensorDir = (dir+1)%4;
int leftWallSensorDir = (dir+3)%4;

//sends over information about walls, direction, and robot presence
byte mazeMsg=dir;
//sends over information about treasure
byte treasureMsg=0;

//SETUP METHODS-----------------------------------------------------------------------------------------------
void servoSetup(){
  Serial.begin(9600);
  servoLeft.attach(4); //left servo
  servoRight.attach(5); //right servo   
}

void wallSetup(){
  pinMode(S0,OUTPUT);//input select signal 0 for mux
  pinMode(S1,OUTPUT);//input select signal 1 for mux  
  pinMode(A5, INPUT);//output of the mux, input into the Arduino. Need a resistor for this
}

void LEDSetup(){
  pinMode(green_led,OUTPUT);
  pinMode(yellow_led,OUTPUT);  
  analogWrite(green_led,LOW);
  analogWrite(yellow_led,LOW);
}

void setupRadio(){
  //Radio stuff
  radio.begin();
  radio.openWritingPipe(pipes[0]);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}


//METHODS FOR TURNING/MOVING FORWARD/STOPPING---------------------------------------------------------------------
//a turn 90 to the right
void right_turn(){
  dir = (dir + 1);//change cardinal direction one to the right
  if(dir>=4) {dir = dir-4;}
  //update the mazeMsg
  
  mazeMsg |= dir;
  servoLeft.write(180);
  servoRight.write(180);
  delay(620); // was 670
}
//a turn 90 degrees to the left
void left_turn(){
  dir = (dir + 3)%4;//change cardinal direction one to the left
  //if dir is negative, fix it so it works in our data scheme
  if(dir>=4) {dir  = dir - 4;}
  //update the mazeMsg
  mazeMsg |= dir;
  servoLeft.write(0);
  servoRight.write(0);
  delay(610); // was 670
}

void uturn(){
  dir = (dir +2)%4;
  if(dir>=4) {dir  = dir - 4;}
  mazeMsg |= dir;
  servoLeft.write(0);
  servoRight.write(0);
  delay(610); // was 670
  servoLeft.write(0);
  servoRight.write(0);
  delay(610); // was 670
}

//correction, not a turn
void slight_right(){
  servoRight.write(92);
  servoLeft.write(93);
  delay(120);
}
//correction, not a turn
void hard_right(){
   servoRight.write(92);
   servoLeft.write(94);
   delay(120);
}
//correction, not a turn
void slight_left(){
  servoRight.write(87);
  servoLeft.write(88);
  delay(120);
}
//correction, not a turn
void hard_left(){
  servoRight.write(86);
  servoLeft.write(88);
  delay(120);
}
//move forward
void forward(){
  servoRight.write(0);
  servoLeft.write(180);  
  mazeMsg |= dir;
}
//stop
void pause(){
  servoRight.write(90);
  servoLeft.write(90);  
}

//LINE SENSOR METHODS-----------------------------------------------------------------------------
//returns true if white, false if not white
bool leftLineSensor(){
  lsensorL = analogRead(A1);
  if(lsensorL<line_threshold){
    return true;
    }
  return false;
}
bool rightLineSensor(){
  lsensorR = analogRead(A3);
  if(lsensorR<line_threshold){
    return true;
    }
  return false;
}
bool middleLineSensor(){
  lsensorM = analogRead(A2);
  if(lsensorM<line_threshold){
    return true;
    }
   return false;
}

//METHODS FOR WALL DETECTION--------------------------------------------------------------------------------
//////////////////////////////////////////
//setup for the mux and the wall sensors
//E AND S2 ARE ALWAYS LOW FOR MUX TO WORK
//      S1     S0      WALL SENSOR
//      L      L          LEFT            
//      L      H          RIGHT            
//      H      L          FRONT  
//////////////////////////////////////////

bool left_wall_detect(){
  //setting mux select signals
  // digitalWrite(S2,LOW);
  l_wall_sensor = 0;
  digitalWrite(S1, LOW);
  digitalWrite(S0, LOW);
  l_wall_sensor = average();
  //Serial.println(l_wall_sensor);
  //Serial.println("left wall sensor: "+String(l_wall_sensor));
  if(l_wall_sensor > l_wall_threshold){
   // Serial.println("Left wall detected");
    wallsToRadio(leftWallSensorDir);
    return true;
  } 
  return false;

}
bool right_wall_detect(){
  //setting mux select signals
  // digitalWrite(S2,LOW);
  r_wall_sensor = 0;
  digitalWrite(S1, LOW);
  digitalWrite(S0, HIGH);
  r_wall_sensor = average();
  //Serial.println("right wall sensor: "+String(r_wall_sensor));
  //Serial.println(r_wall_sensor);
  if(r_wall_sensor>r_wall_threshold){
    //Serial.println("Right wall detected");
    wallsToRadio(rightWallSensorDir);
    return true;
  } 
  return false;
}
bool front_wall_detect(){
  //setting mux select signals
  // digitalWrite(S2,LOW);
  f_wall_sensor = 0;
  digitalWrite(S1,HIGH);
  digitalWrite(S0,LOW);
  f_wall_sensor=average();
  //Serial.println(f_wall_sensor);
  //Serial.println(f_wall_sensor);
  if(f_wall_sensor>f_wall_threshold){
    //Serial.println("front wall detected");
    //wall information to mazeMsg
   wallsToRadio(dir);
    return true;
  } 
  return false;
}

int average(){
  avg_reading = 0;
    for(int i = 0; i < 100; i = i+1){
       avg_reading = avg_reading + analogRead(A5);
    }
    avg_reading = avg_reading/100;
    return abs(avg_reading);
}


//SEND WALL INFO VIA RADIO-----------------------------------------------------------------------
void wallsToRadio(int d){
  //bits[7:0]
  //north wall, bit 5
  if(d==0){
    mazeMsg |= 0b00100000;
  }
  //east wall, bit 4
  else if(d==1){
    mazeMsg |= 0b00010000;
  }
  //south wall, bit 3
  else if(d==2){
    mazeMsg |= 0b00001000;
  }
  //west wall, bit 2
  else if(d==3){
    mazeMsg |= 0b00000100;
  }
}
//LINE FOLLOWING--------------------------------------------------------------------------------
void lineFollow(){
  //slight left correction
  if(leftLineSensor() && !rightLineSensor()&& middleLineSensor()) slight_left();
  //slight right correction
  if(!leftLineSensor() && rightLineSensor()&& middleLineSensor()) slight_right();
  //hard left correction
  if(leftLineSensor() && !rightLineSensor()&& !middleLineSensor()) hard_left();
  //hard right correction
  if(!leftLineSensor() && rightLineSensor()&& !middleLineSensor()) hard_right();
  //go forward
  if(!leftLineSensor() && !rightLineSensor()&& middleLineSensor()) forward();
       
}

//RIGHT WALL FOLLOW-----------------------------------------------------------------------------------------
void rightWallFollow(){
      //no wall in front, go forward
      front_wall_detect();
      right_wall_detect();
      left_wall_detect();
      if(!front_wall_detect()){
            forward();  
            //Serial.println("case 1\n");
          }
          //if you can turn right, then do it
      else if(!right_wall_detect()){
        right_turn();
        //Serial.println("case 2\n");
        }
      //wall on front and right, turn left
      else if(!left_wall_detect() && front_wall_detect() && right_wall_detect()){
        //digitalWrite(green_led, HIGH);
        left_turn();
        //Serial.println("case 3\n");
        //digitalWrite(green_led, LOW);
      }
   //walls on left and front
      else if(left_wall_detect() && !right_wall_detect() && front_wall_detect()){
        //digitalWrite(green_led, HIGH);
        right_turn();
        //Serial.println("case 4\n");
        //digitalWrite(green_led, LOW);
      }
      //walls everywhere. Uturn?
      else if(left_wall_detect() && right_wall_detect() && front_wall_detect()){
        //digitalWrite(green_led, HIGH);
        uturn();
        //Serial.println("case 5\n");
        //digitalWrite(green_led, LOW);
      }
      
}

//SEND RADIO INFORMATION-------------------------------------------------------------------------------------------
bool sendRadio(){
    //[NorthWall EastWall SouthWall WestWall 2bitsForDirection]
    //1 means wall present, 0 means absent
    //00 is head north
    //01 is head east
    //10 is head south
    //11 is head west 
  
  char msg[2] ={char(treasureMsg), char(mazeMsg)};
  
  bool ok=radio.write( msg, 2);
  if(ok){
    //Serial.println("ok");
    //Serial.print("mazeMsg: ");
  //Serial.println(mazeMsg,BIN);
  //Serial.println();
    }
    
  else{//Serial.println("failed");
    }
  
  return ok;
}

//INTERSECTION----------------------------------------------------------------------------
void atIntersection(){
  //if intersection
  
  if(leftLineSensor() && rightLineSensor() && middleLineSensor()){
    forward();//this pause and delay is for moving forward after intersection
    delay(330);
    pause();
    
    //Serial.println(mazeMsg);
    resetMazeMsg();
    pause();//this pause and delay is for reading wall time
    delay(200);
    
    rightWallFollow();
    pause();
    int failCount = 0;
    bool ok = false;
    while(ok==false && failCount<4){
      ok=sendRadio();
      failCount++;
    }
    if(failCount>=4){
      setupRadio();
      sendRadio();
      sendRadio();  
    }
    
  }
}

//resets the maze message to send over radio back to 0
void resetMazeMsg(){
  treasureMsg = 0;
  mazeMsg = 0;
}

//SETUP--------------------------------------------------------------------------------------------------------
void setup() {
  servoSetup();
  pause();
  wallSetup();
  setupRadio();
  //LEDSetup();
  resetMazeMsg();
 
}

//LOOP-------------------------------------------------------------------------------------------------------
void loop() {
    atIntersection();
    lineFollow();
    
}


// update robot position and squares visited
void updatePosition() {
    if (dir == north) {
      row=row-1;
      visited[row+1][col] = 1;
    }
    else if (dir == south) {
      row=row+1;
      visited[row-1][col] = 1;
    }
    else if (dir == east) {
      col=col+1;
      visited[row][col-1] = 1;
    }
    else if (dir == west) {
      col=col-1;
      visited[row][col+1] = 1;
    }
}

//face the robot in the desired cardinal direciton
void faceDir(int cardinal){
  if(dir==cardinal){
    //do nothing  
    return;
  }
  else if((cardinal-dir+4)%4 == 1){
    right_turn();
  }
  else if(abs(dir-cardinal)==2){
    uturn();  
  }
  else{
    left_turn();
  }
}

//trying out dfs
void dfs(){
  //detect walls
  front_wall_detect();
  right_wall_detect();
  left_wall_detect();
  //given walls, choose a direction to go SENW priority

  //if not bottom row and no wall to othe south and south is unvisited, then visit
  if(row<8 && (mazeMsg & 0b00001000 == 0) && visited[row+1][col]==0){
    faceDir(south);
    stack.push(dir);
  }
  

  //if not rightmost col and no wall to the east and east is unvisited, then visit
  else if(col<8 && (mazeMsg & 0b00010000 == 0) && visited[row][col+1]==0){
    faceDir(east);
    stack.push(dir);  
  }

  //if not top row and no wall to the north and north is unvisited, then visit
  else if(row>0 && (mazeMsg & 0b00100000 == 0) && visited[row-1][col]==0){
      faceDir(north);
      stack.push(dir);
   }

  else if(col>0 && (mazeMsg & 0b00000100 ==0) && visited[row][col-1]==0){
    faceDir(west);
    stack.push(dir);  
  }
  //all else fails go the opposite direction of most recent dir
  else{
    int newDir = (stack.pop() + 2) % 4;
    faceDir(newDir);
  }
  
  //update position and visited tiles
  updatePosition();
}













