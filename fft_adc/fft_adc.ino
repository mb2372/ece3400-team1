/*
fft_adc.pde
guest openmusiclabs.com 8.18.12
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.  there is a pure data patch for
visualizing the data.
*/

#define LIN_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library

void setup() {
  Serial.begin(115200); // use the serial port
  pinMode(LED_BUILTIN, OUTPUT);
}
int j=0;
void loop() {
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      fft_input[i] = analogRead(A0); // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_lin(); // take the output of the fft
    Serial.println("Starting");
    for (byte i=0; i<FFT_N/2;i++){
      String str = "Bin Number ";
      String str2 = str + i +": ";
      Serial.println(str2);  
      Serial.println(fft_lin_out[i]);
      if(fft_lin_out[19]>=15){
        digitalWrite(LED_BUILTIN, HIGH);
        }
      else{digitalWrite(LED_BUILTIN, LOW);}
      }
  }
