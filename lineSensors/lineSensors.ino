#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
#include <FFT.h> // include the FFT library
#include <Servo.h>
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
//  int S2 = 5; //pin 5 is S2

// led pins
int yellow_led = 2;
int green_led = 1; 

//IR stuff
double ir_sensor;
double ir_threshold = 110.0;

unsigned int tempADCSRA=0;
unsigned int tempTIMSK0=0;
unsigned int tempADMUX=0;
unsigned int tempDIDR0=0;

void setup() {
  tempADCSRA=ADCSRA;
  tempTIMSK0=TIMSK0;
  tempADMUX=ADMUX;
  tempDIDR0=DIDR0;
  servoLeft.attach(9); //left servo
  servoRight.attach(10); //right servo 
  pinMode(LED_BUILTIN,OUTPUT);//testing when we are at intersection or correcting. This can be removed later  
  pinMode(S0,OUTPUT);//input select signal 0 for mux
  pinMode(S1,OUTPUT);//input select signal 1 for mux
  // pinMode(S2,OUTPUT);//input select signal 1 for mux
  pinMode(green_led,OUTPUT);
  pinMode(yellow_led,OUTPUT);
  pinMode(A5, INPUT);//output of the mux, input into the Arduino. Need a resistor for this
  //Serial.begin(9600);//for testing the wall output, can delete later
}

//TODO: METHOD FOR CHECKING MICROPHONE TO KNOW WHEN WE START

//---------------------------------------------------------------------------------------------------------------

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
  // digitalWrite(S2,LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S0, LOW);
  l_wall_sensor = analogRead(A5);
}
void right_wall_detect(){
  //setting mux select signals
  // digitalWrite(S2,LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S0, HIGH);
  r_wall_sensor = analogRead(A5);
}
void front_wall_detect(){
  //setting mux select signals
  // digitalWrite(S2,LOW);
  digitalWrite(S1,HIGH);
  digitalWrite(S0,LOW);
  f_wall_sensor = analogRead(A5);
}


// A5 is h l h
// A7 is h h h 

//---------------------------------------------------------------------------------------------------------------

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


//---------------------------------------------------------------------------------------------------------------
//METHOD FOR IR
void ir_read(){
    TIMSK0 = 0; // turn off timer0 for lower jitter
    ADCSRA = 0xe5; // set the adc to free running mode
    ADMUX = 0x40; // use adc0
    DIDR0 = 0x01; // turn off the digital input for adc0
    //cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    //sei();//interrupts
    ir_sensor=fft_log_out[43];
    //Serial.println("start");
    /*for (byte i = 0 ; i < FFT_N/2 ; i++) { 
      Serial.println("Bin Number: " + i);
      Serial.println(fft_log_out[i]); // send out the data
    }*/
    ADCSRA = tempADCSRA;
    TIMSK0 = tempTIMSK0; // turn off timer0 for lower jitter
    ADMUX = tempADMUX; // use adc0: analog A0
    DIDR0 = tempDIDR0;
}
//---------------------------------------------------------------------------------------------------------------

void loop() {
  lsensorR = analogRead(A3);
  lsensorL = analogRead(A1);
  lsensorM = analogRead(A2);

  ir_read();
  //if IR detected, stop moving (for now)
  if(ir_sensor>=ir_threshold){
    digitalWrite(yellow_led, HIGH);
    pause(); 
    delay(3000);
    digitalWrite(yellow_led, LOW); 
  }
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
        digitalWrite(green_led, HIGH);
        right_turn();
        digitalWrite(green_led, LOW);
      }
      //no wall on the left and wall in front (If can turn left and right, will turn left)
      if(l_wall_sensor<wall_threshold && f_wall_sensor>=wall_threshold){
        digitalWrite(green_led, HIGH);
        left_turn();
        digitalWrite(green_led, LOW);
      }
      //walls everywhere. Uturn?
      if(l_wall_sensor>=wall_threshold && r_wall_sensor>=wall_threshold && f_wall_sensor>=wall_threshold){
        digitalWrite(green_led, HIGH);
        left_turn();
        left_turn();
        digitalWrite(green_led, LOW);
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
  // digitalWrite(S2,HIGH);
  digitalWrite(S1,HIGH);
  digitalWrite(S0,HIGH);

}

