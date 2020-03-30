## UCLA CS 32 Winter 2020 Project 4 - ~~GooberEats~~ QuarantEats

Oh UCLA student! In these trying times of quarantine, why not have your food delivered by robot?

This program takes as input real street data of Westwood, CA in `mapdata.txt` (derived and simplified from OpenStreetMap data) and `deliveries.txt`, a formatted list of food to deliver at certain locations. It then finds the optimally shortest routes between locations by using A* search, and attempts to optimize the delivery version of the Traveling Salesman Problem with the greedy Nearest Neighbor algorithm. By storing the street map data in a custom implemented hash map, this program operates quickly, so those De Neve Late Night chicken tendies can practicually jump into your tummy.

My implementation of what was required in the spec is contained in the `ExpandableHashMap.h, StreetMap.cpp, PointToPointRouter.cpp, DeliveryOptimizer.cpp`, and `DeliveryPlanner.cpp` files.

### How to Run the Program
Download the `skeleton.zip` file. Replace the existing files with the files listed above. Build the program and run the executable from the command line, like so:
```
# cd to the directory of source files
g++ -std=c++11 *.cpp -o QuarantEats
./QuarantEats mapdata.txt deliveries.txt
```
The user can change `deliveries.txt` to contain whatever list of items they desire. Just check `main.cpp` to understand the format. (The delivery locations must be at street corners or known geographic coordinates found in `mapdata.txt`)
