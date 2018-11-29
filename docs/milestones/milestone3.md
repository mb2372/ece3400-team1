#Milestone 3
The goal of this lab is to create a search algorithm for robot to explore the entire maze. 
Our group implemented a depth first search algorithm to traverse the maze.

##Data Structures
We first create a boolean matrix called visited that is the size of the maze. A value of true in a given slot means that the node had been visited, and a value of false in a given slot indicates that the node had not yet been visited. The algorithm only stops when all values in the matrix are true.

A stack is used to store which node to visit next. Once the next node to visit is decided at an intersection, the next node is added to the stack. If the robot reaches a point in the maze where all the surrounding nodes have been visited, then the stack is popped and the robot returns to a previous node. This continues until an unvisited node is reached.

##Algorithm
The algorithm itself starts with the robot detecting the presence of walls, which cardinal directions the walls are at, and updating the relevant variables for radio transmission to the base station. The algorithm then checks where the robot currently is as well as the visited matrix to see which adjacent nodes have not been visited yet. The algorithm also checks for where walls are, and eliminates some adjacent nodes as possible next nodes to visit as our robot cannot quantum tunnel through the walls. If there are multiple choices for where to travel next, our group decided a preference order to handle these situations. Once a valid, reachable node is decided, the robot orients itself properly to travel to the next node. This is done through the following method:
```cpp
//face the robot in the desired cardinal direciton
void turnToDir(int cardinal){
  if(dir==cardinal){//if already oriented in the proper direction, go forward
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
```

The node is then pushed onto the stack. If the robot is ever surrounded by already visited nodes, then the stack is popped and the robot backtracks until there are unvisited adjacent nodes again.


The following video shows the robot exploring a 2x3 maze while updating the GUI correctly in real time.
<iframe width="640" height="480" src="https://www.youtube.com/embed/kUjGW06mavE" frameborder="0" allowfullscreen></iframe>