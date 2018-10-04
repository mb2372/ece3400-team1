# Lab 2: Analog Circuitry and FFT

## Goal/ Objective

The purpose of this lab is to add sensors to our robot so that it can detect sound waves and infrared waves. A microphone circuit is added to detect a 660Hz whistle blow, signifying the start of the maze mapping. Another circuit is added to detect infrared (IR) waves emitted from other robots at around 6kHz. This circuit also differentiates between real robots emitting 6kHz as well as decoy robots emitting 18kHz.

## Prelab

To perform Fast Fourier Transform (FFT) analysis, we used Open Music Labs Arduino FFT library. The library uses the Arduino's Analog to Digital Converter (ADC) as opposed to the Arduino's analogRead function. The analogRead function samples at a frequency that is adequate for the 660Hz sound wave, but its sampling frequency is not high enough for the 6kHz infrared (IR) waves. Thus, the ADC is needed for FFT analysis of IR signals. Filters will also need to be added so that ambient noise and light from people and the environment do not interfere with these FFT readings. 

To ensure that the FFT library provided was functioning correctly, we performed FFT analysis on signals at 660Hz, 2kHz, and 20kHz. We expected these to have peaks at different bins. The FFT analysis is shown below:


![](images/lab2/660hz_fft_prelab.jpeg){:height="480px" width="640px"}

![](images/lab2/2khz_fft_prelab.jpeg){:height="480px" width="640px"}

![](images/lab2/20khz_fft_prelab.jpeg){:height="480px" width="640px"}


## Optical Team: IR Circuit

