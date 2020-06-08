# intelligentSearch
Solving the 8-Puzzle with AI using different heuristic functions.  

This project uses an A* search implementation using three different heuristic functions to sole the 8-Puzzle. After running 
a heuristic function, the efficiencey of the function is printed with the metrics of execution time, the number of nodes 
generated, the number of nodes expanded, the depth of the tree, the branching factor (nodes generated / depth ) and the 
memory used. 

Each step is printed out to the console.

There are two initial states programed into the code:

State 1:        State 2:

2 8 3          2 1 6
1 6 4          4   8
  7 5          7 5 3
  
  
The goal state for the puzzle is:

1 2 3
8   4
7 6 5
