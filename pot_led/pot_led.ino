#include <Servo.h>
Servo servo1;
void setup() {
  //initializing the serial port with 9600 baud
  Serial.begin(9600);
  servo1.attach(9);
}

void loop() {
  //reading the value from the potentiometer
  float value=analogRead(A0);
  //printing the value to the serial monitor
  Serial.print("Analog Pot= ");
  Serial.println(value);
  //mapping the value to the LED
  int input=map(value, 0, 1014, 0, 180);
  Serial.print("PWM= ");
  Serial.println(input);
  //writing the value to the LED/Servo
  servo1.write(input);
  delay(1000);  
}
