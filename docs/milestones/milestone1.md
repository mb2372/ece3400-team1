#Milestone 1

##Introduction
To meet Milestone 1, the robot must be able to complete a figure eight. In order to accomplish this goal, the robot must be able to follow a white line, and turn at intersections. The main difficulties in this milestone were programming crisp turns. Most of our attention was focused on software this lab.

##Hardware
Our main focus for this milestone was the implementation of our line following sensors. We chose to put three sensors in the front of our robot with the sensors in a line perpendicular to the line they are following. This way, the middle sensor is always over the line being followed while the two outer sensors, which are straddling the line, detect if the robot is straying from the line or if the robot has arrived at an intersection.

An abstraction of the sensor setup is shown below:
![](images/milestone1images/sensor_diagram.jpg){:height="480px" width="640px"}

The actual sesnor setup on the robot is shown below: 
![](images/milestone1images/sensor_on_robot.jpg){:height="480px" width="640px"}