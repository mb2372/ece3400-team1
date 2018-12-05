#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
              
RF24 radio(9, 10); // CE, CSN
long long address = 0x0000000002LL;

String guiMsg = "";

//starting coordinates (row, col)
int row = 0;
int col = 0;
              
void setup() {
  Serial.begin(9600);
  while (!Serial) {
  ; // Wait for serial port to connect. Needed for native USB port only
  }
  setupRadio();  
         
}

void loop() {
//int msg[1];
if (radio.available()) {
   char msg[32]="";
   radio.read(msg, sizeof(msg));
   guiMsg = "";   

  //treasure info has info on treasure tbh
  byte treasureInfo = msg[0];

   //maze info has direction, walls, robot               
   byte mazeInfo = msg[1];  
   //get direction data from robot and update position
   byte dir = mazeInfo;
   dir &= 0b00000011;
   //get wall data from robot
   byte walls = mazeInfo;
   walls &= 0b00111100;
   updateWalls(walls);

   //get robot presence
   byte roboPresence = mazeInfo;
   roboPresence&= 0b01000000;
   updateRoboPresence(roboPresence);

   //get treasure information
   byte tshape = treasureInfo;
   tshape &= 0b00000110;
   updateTShape(tshape);

   //get color only if treasure is present
   byte tcolor = treasureInfo;
   tcolor &= 0b00000001;
   if(tshape!=0){//if there is a shape
      updateTColor(tcolor);
    }
   //update the guiMsg
   guiMsg = String(row)+","+String(col) + guiMsg;            
   Serial.println(guiMsg);
   
    updatePosition(dir);
   
   //Serial.println("t info: "+String(treasureInfo));
   //Serial.println("maze info: "+String(mazeInfo));
   //Serial.println();
   delay(500);               
   }
}

 /**** SET UP ****/

void setupRadio(){
   radio.begin();
   radio.openReadingPipe(0, address);
   radio.setPALevel(RF24_PA_MAX);
   radio.startListening();
}

//update the (row,col) coordinates based on which direction we are going in
//bits 1 and 0 of [7:0]
void updatePosition(byte d){
   switch(d){
   case 0://heading north, row decrements by 1
     row = row-1;
     //Serial.println("heading north");
     break;
   case 1://heading east, col increments by 1
     col = col+1;
     //Serial.println("heading east");
     break;
   case 2://heading south, row increments by 1
     row = row+1;
     //Serial.println("heading south");
     break;
   case 3://heading west, ,col dec by 1
     col = col -1;
     //Serial.println("heading west");
     break;
   }
   
   
}

//update the presence of walls for this tile
//bits [5:2] of [7:0]
void updateWalls(byte wall){
  byte n = (wall & 0b00100000);
  byte e = (wall & 0b00010000);
  byte s = (wall & 0b00001000);
  byte w = (wall & 0b00000100);
  //Serial.println(wall,BIN);
  //if north. north is bit 5 of [7:0]
  if( n== 0b00100000){
    //Serial.println("N wall");
    guiMsg = guiMsg + ",north=true";
  }
  //if east. east is bit 4 of [7:0]
  if( e== 0b00010000){
    //Serial.println("E wall");
    guiMsg = guiMsg + ",east=true";
  }
  //if south. south is bit 3 of [7:0]
  if( s== 0b00001000){
    //Serial.println("S wall");
    guiMsg = guiMsg + ",south=true";
  }
  //if west. west i bit 2 of [7:0]
  if( w== 0b00000100){
    //Serial.println("W wall");
    guiMsg = guiMsg + ",west=true";
    }
}

//updates to show if there is another robot present
//bit [6] of [7:0]
void updateRoboPresence(byte robot){  
  if(robot && 0b01000000){
    guiMsg = guiMsg + ",robot=true";  
  }
}

//sees what treasure shape we are seeing
//uses bits 10 and 9 or [2:1] on the first byte
void updateTShape(byte shape){
  //no treasure
  if(shape && 0b00000110 == 0){
    ;//already false no need to do anything
  }
  //triangle
  else if(shape && 0b00000110 == 0b00000010){
    guiMsg = guiMsg + ",tshape=Triangle";
  }
  //square
  else if(shape && 0b00000110 == 0b00000100){
    guiMsg = guiMsg + ",tshape=Square";
  }
  //diamond
  else if(shape && 0b00000110 == 0b00000110){
    guiMsg = guiMsg + ",tshape=Diamond";
  }
}


//updates the color of the treasure
//bit 8 overall and bit 0 of the first byte
void updateTColor(byte color){
  if(color==1){
    guiMsg = guiMsg+",tcolor=Blue";  
  }  
  else if(color==0){
    guiMsg = guiMsg+",tcolor=Red";  
  }
}

            
