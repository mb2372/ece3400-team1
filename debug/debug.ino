#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the FFT library
#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <StackArray.h>

bool IR = false;
//DFS data structures------------------------------------------------------------
StackArray<int> stack;
int row = 0;
int col = 0;
int numRows = 9;
int numCols = 9;


boolean visited[9][9] = {
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}, 
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}, 
};

/*
boolean visited[2][3] = {
  {0,0,0},
  {0,0,0},
};
*/

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
int line_threshold=945; //less than is white, greater than is black



//WALLS-------------------------------------------------------------------------------------
//declaring wall sensors & wall/no wall threshold
int l_wall_sensor; //left wall sensor
int r_wall_sensor; //right wall sensor
int f_wall_sensor; //front wall sensor
int f_wall_threshold=130;//less is no wall, > is wall
int l_wall_threshold = 170;
int r_wall_threshold = 170;

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
byte mazeMsg=0;
//sends over information about treasure
byte treasureMsg=0;

//microphone stuff. In analog A4---------------------------------------------------------------------------------
int mic;
int mic_threshold = 43;

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
  rightWallSensorDir = (dir+1)%4;
  leftWallSensorDir = (dir+3)%4;
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
  rightWallSensorDir = (dir+1)%4;
  leftWallSensorDir = (dir+3)%4;
  //update the mazeMsg
  mazeMsg |= dir;
  servoLeft.write(0);
  servoRight.write(0);
  delay(610); // was 670
}

void uturn(){
  dir = (dir +2)%4;
  if(dir>=4) {dir  = dir - 4;}
  rightWallSensorDir = (dir+1)%4;
  leftWallSensorDir = (dir+3)%4;
  
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
   servoLeft.write(98);
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
  rightWallSensorDir = (dir+1)%4;
  leftWallSensorDir = (dir+3)%4;
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
  rightWallSensorDir = (dir+1)%4;
  leftWallSensorDir = (dir+3)%4;
  
  l_wall_sensor = 0;
  digitalWrite(S1, LOW);
  digitalWrite(S0, LOW);
  delay(80);
  l_wall_sensor = average();
  //Serial.println(l_wall_sensor);
  //Serial.println("left wall sensor: "+String(l_wall_sensor));
  if(l_wall_sensor > l_wall_threshold){
    //Serial.println("Left wall detected");
    wallsToRadio(leftWallSensorDir);
    return true;
  } 
  return false;

}
bool right_wall_detect(){
  //setting mux select signals
  // digitalWrite(S2,LOW);
  rightWallSensorDir = (dir+1)%4;
  leftWallSensorDir = (dir+3)%4;
  
  r_wall_sensor = 0;
  digitalWrite(S1, LOW);
  digitalWrite(S0, HIGH);
  delay(80);
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
  delay(80);
  f_wall_sensor=average();

  if(f_wall_sensor>f_wall_threshold){
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
  if(d==north){
    mazeMsg |= 0b00100000;
    //Serial.println("N wall");
  }
  //east wall, bit 4
  if(d==east){
    mazeMsg |= 0b00010000;
    //Serial.println("E wall");
  }
  //south wall, bit 3
  if(d==south){
    mazeMsg |= 0b00001000;
    //Serial.println("S wall");
  }
  //west wall, bit 2
  if(d==west){
    mazeMsg |= 0b00000100;
    //Serial.println("W wall");
  }
}
//LINE FOLLOWING--------------------------------------------------------------------------------
void lineFollow(){
  //true if white and false if not white
  
  //slight left correction
  //if(leftLineSensor() && !rightLineSensor()&& middleLineSensor()) slight_left();
  //slight right correction
  //if(!leftLineSensor() && rightLineSensor()&& middleLineSensor()) slight_right();
  //hard left correction
  //if(leftLineSensor() && !rightLineSensor()&& !middleLineSensor()) hard_left();
  //hard right correction
  //if(!leftLineSensor() && rightLineSensor()&& !middleLineSensor()) hard_right();

  //left is white and right is not white, need to tilt left
  if(leftLineSensor() && !rightLineSensor()){
    slight_left();
    } 
  //left is not white and right is white, need to tilt right
  if(!leftLineSensor() && rightLineSensor()){
    hard_right();
    }
  
  //go forward
  if(!leftLineSensor() && !rightLineSensor()&& middleLineSensor()){
    forward();
    } 
  if(!leftLineSensor() && !rightLineSensor()&& !middleLineSensor()){
    forward();
    } 
      
}



//METHOD FOR CHECKING MICROPHONE TO KNOW WHEN WE START
//---------------------------------------------------------------------------------------------------------------
void mic_read(){
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      fft_input[i] = analogRead(A4); // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  mic = fft_log_out[19];//set the sampled value to mic
}



// update robot position and squares visited-----------------------------------------------------------------
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

//face the robot in the desired cardinal direciton-------------------------------------------------------------
void turnToDir(int cardinal){
  if(dir==cardinal){
    //do nothing  
    //return;
    forward();
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

//trying out dfs-------------------------------------------------------------------------------------------
void dfs(){
  //detect walls
  bool f = front_wall_detect();
  //Serial.println("Front Wall: " + String(f)); 
  //Serial.println("Front Wall Sensor: "+String(f_wall_sensor));
  //delay(10);
  
  bool r = right_wall_detect();
  //Serial.println("Right Wall: " + String(r));
  //Serial.println("Right Wall Sensor: "+String(r_wall_sensor));
  //delay(10);
  bool l = left_wall_detect();
  //Serial.println("Left Wall: " + String(l));
  //Serial.println("Left Wall Sensor: "+String(l_wall_sensor));
  //Serial.println("");
  //delay(10);
  
  //given walls, choose a direction to go SENW priority
  //if not bottom row and no wall to othe south and south is unvisited, then visit
 // Serial.println(mazeMsg,BIN);
  byte n = (mazeMsg & 0b00100000);
  byte e = (mazeMsg & 0b00010000);
  byte s = (mazeMsg & 0b00001000);
  byte w = (mazeMsg & 0b00000100);
/*
  if(row<numRows-1 && (s == 0) && visited[row+1][col]==0 && dir==south){
    turnToDir(south);
    stack.push(dir);
  }
  
  //if not rightmost col and no wall to the east and east is unvisited, then visit
  else if(col<numCols-1 && (e == 0) && visited[row][col+1]==0 && dir==east){
    turnToDir(east);
    stack.push(dir);  
  }
  //if not top row and no wall to the north and north is unvisited, then visit
  else if(row>0 && (n == 0) && visited[row-1][col]==0 && dir ==north){
      turnToDir(north);
      stack.push(dir);
   }

  else if(col>0 && (w ==0) && visited[row][col-1]==0 && dir==west){
    turnToDir(west);
    stack.push(dir);  
  }

 */
  if(row<numRows-1 && (s == 0) && visited[row+1][col]==0){
    turnToDir(south);
    stack.push(dir);
  }
  
  //if not rightmost col and no wall to the east and east is unvisited, then visit
  else if(col<numCols-1 && (e == 0) && visited[row][col+1]==0){
    turnToDir(east);
    stack.push(dir);  
  }
  //if not top row and no wall to the north and north is unvisited, then visit
  else if(row>0 && (n == 0) && visited[row-1][col]==0){
      turnToDir(north);
      stack.push(dir);
   }

  else if(col>0 && (w ==0) && visited[row][col-1]==0){
    turnToDir(west);
    stack.push(dir);  
  }
  //all else fails go the opposite direction of most recent dir
  else{
    if(!stack.isEmpty()){
       int newDir = (stack.pop() + 2) % 4;
       turnToDir(newDir);
    }
    else{
      forward();  
    }
  }

  //update position and visited tiles
  updatePosition();
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
    digitalWrite(2, HIGH);
    forward();//this pause and delay is for moving forward after intersection
    delay(330);
    pause();
    //Serial.println(mazeMsg);
    resetMazeMsg();
    //pause();//this pause and delay is for reading wall time
    //delay(20);
    
    dfs();
    ir_detect();
    while(IR==true){
      pause();  
      ir_detect();
    }
    //rightWallFollow();
    pause();
    int failCount = 0;
    bool ok = false;
    while(ok==false && failCount<5){
      ok=sendRadio();
      failCount++;
    }
    if(failCount>=5){
      setupRadio();
      sendRadio();
      sendRadio();  
    }
    digitalWrite(2, LOW);
  }
  
}

//resets the maze message to send over radio back to 0
void resetMazeMsg(){
  treasureMsg = 0;
  mazeMsg = 0;
}

//IR
void ir_detect(){
  double ir_val=analogRead(A0);
  if(ir_val<650){
    pause();
    IR = true;
    //updatePosition();
    }
   else{
    IR = false;
    }
  }


boolean starting = false;//we have not started yet
//SETUP--------------------------------------------------------------------------------------------------------
void setup() {
  servoSetup();
  pause();
  wallSetup();
  setupRadio();
  //LEDSetup();
  resetMazeMsg();
  
  //first have to wait for the microphone signal
  while(starting==false){//loop while we have not started
    pause();
    mic_read();//update mic val
    if(mic>mic_threshold){//if threshold is met
        starting=true;//condition to leave while loop
      }
  }
}

//LOOP-------------------------------------------------------------------------------------------------------
void loop() {
    ir_detect();
    while(IR==true){
      pause();  
      ir_detect();
    }
    atIntersection();
    ir_detect();
    while(IR==true){
      pause();  
      ir_detect();
    }
    lineFollow();
    
}


