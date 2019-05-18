# pathfinderAstar

Implemented by C.
Using A-star alg.

# compile:
gcc main.c -o pathfinder

# run:
- ./pathfinder [inputfile] [starting-y] [starting-x] [ending-y] [ending-x] 
- eg:
./pathfinder exampleMap.txt 0 3 5 0

~~~text
Take 5 argument to run.
argv[1]: input filename;
argv[2]: y coordinate of starting point;
argv[3]: x coordinate of starting point;
argv[4]: y coordinate of ending point;
argv[5]: x coordinate of ending point;
~~~

# format of input file:
- first line is [width] and [height]. seperated by space or tab;
- following lines are filled with space-or-tab-seperated 0s and 1s;
- 0 represent as free space can move. 1 indicate space is occupied;
- eg: 
~~~text
5 3
0 1 1 0 0 
1 0 0 0 0 
0 1 0 0 0
~~~

# format of stdout
- first line: the number of nodes has been explored. and the cost of shortest path. seperated by space;
- second line: the actions to get to destination (ending point). (U D L R; up down left right correspondingly). seperated by space;
~~~text
if not reachable
- first line: the number of nodes has been explored. and the cost of shortest path which is 0. seperated by space;
- second line: 'X' (indicate no valid path);
~~~
