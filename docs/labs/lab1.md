# Lab 1: Microcontroller

### Objectives
* Learn how to use the various functionalities of the Arduino Uno and the Arduino IDE.
* Construct a simple functional Arduino program using multiple external components and the Arduino Uno.
* Assemble our robot and have it perform a simple autonomous task

### Subteams
**Team 1**: Manhal, Grant

**Team 2**: Jason, Beau

## Blinking an internal LED
After downloading the Arduino IDE from [this link](https://www.arduino.cc/en/Main/Software), we used the Blink example that can be accessed from File>>Examples>>01.Basics>>Blink to successfully blink the internal LED.
<iframe width="640" height="480" src="https://www.youtube.com/embed/14P1Wfs-y9U" frameborder="0" allowfullscreen></iframe>

## Blinking an external LED
We connected the ouptput pin 12 of the Arduino with the LED, in series with a 1.2 kΩ resistor. The longer leg of the LED is the positive lead (anode) and should be connected with the resistor (and thereby pin 12). The shorter, negative side should connect to GND of the arduino.
The setup is shown in the picture below:
![](./images/lab1/1.jpg)  
