# Lab 3

## Radio Group

We first used a breakout board and soldered the radio pins accordingly. The RF24 library was used and the sample code provided in the library was used to ensure that the radios functioned properly. Data about the maze needs to be sent from the radio on the robot to the radio attached to the Arduino connected to a group member’s laptop. To concisely send this information, bitmasking was used to convey information about walls, other robots, explored tiles, treasures, and which direction the robot is moving in. The encoding is shown below.

 
```cpp
//bitmasks for encoding information
uint16_t message = 0;//default message to send
uint16_t head_north = 0;//bit 10 and 9 is 00
uint16_t head_east = (1<<9);//bit 10 and 9 is 01
uint16_t head_south = (1<<10);//bit 10 and 9 is 10
uint16_t head_west = 0b11000000000;//bit 10 and 9 is 11
uint16_t robot_presence= (1<<8); //1 is robot present, 0 is no robot
uint16_t explored = (1<<7);//1 is space explored, 0 is unexplored
//default value is no treasure, having 00 for bit 6 and bit 5
uint16_t treasure_square = 0b0100000;
uint16_t treasure_diamond = 0b1000000;
uint16_t treasure_triangle = 0b1100000;
uint16_t treasure_color = (1<<4); //1 for red and 0 for blue
//NESW walls. 1 is having a wall present. 0 is no wall present
uint16_t north_wall = (1<<3);
uint16_t east_wall = (1<<2);
uint16_t south_wall = (1<<1);
uint16_t west_wall = 1;

```

Whenever the robot reaches an intersection, the relevant sensor information will be collected and sent out by using the OR operator on the correct bit mask and the message. For instance, if there were walls to the north and east, then the OR operator would be used on message and north_wall. Then the result would be OR-ed with east_wall.

Modifying the sample code to send over our data instead of the default values, we then set up the radio connected to the robot’s Arduino to transmit data and the radio connected to the laptop to receive the data. The radio receiving the data (the base-station) decoded the message by going through the encoded message bit by bit. Each 1 and 0 sent over was then translated into a string that the provided GUI takes in as a parameter. The string was then output using a simple Serial.println() command so that the GUI would update. The base-station received robot data wirelessly, decoded it, and updated the maze on the GUI.

<iframe width="640" height="480" src="https://www.youtube.com/embed/lBzyr6NAHM0" frameborder="0" allowfullscreen></iframe>


## Robot Group

We previously made the microphone work in lab 2. The goal of this lab was to integrate it with the robot so that the arduino would only start moving upon receiving a 660Hz Signal. We used our old function from lab 2:

```cpp
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
```

This function analyzes the analog input into A4 and assigns the value from bin 19 to a variable named ‘mic’. The next step is to pause the robot until ‘mic’ goes over a certain threshold. 

```cpp
boolean starting = false;//we have not started yet
void loop() {
  //first have to wait for the microphone signal
  while(starting==false){//loop while we have not started
	pause();
	mic_read();//update mic val
	if(mic>mic_threshold){//if threshold is met
    	starting=true;//condition to leave while loop
  	}
  }
```

This is accomplished with a boolean value that indicates what stage the robot is in. Before the 660 Hz signal, ‘starting’ is false. With in loop() there is a while loop that will stop upon ‘starting’ becoming true. This only happens when ‘mic’ goes over the designated threshold. The below video demonstrates the robot starting at 660Hz, stopping upon seeing a IR hat, and ignoring a decoy. The last two accomplishments are a result of milestone 2. 


<iframe width="640" height="480" src="https://www.youtube.com/embed/AFjFPb3GQew" frameborder="0" allowfullscreen></iframe>
