# C++/SFML Ant Simulation

This program tries to emulate the behavior of a colony of ants.

<p align="center">
  <img src="https://i.imgur.com/47u7g6c.png"/>
</p>
<p align="center">
  <img src="https://i.imgur.com/c6V7V1o.png"/>
</p>

## How it works
The number of ants is predefined, and the nest is located at (0, 0). When the program begins, ants immediately start randomly moving. While they move, they release a "to nest" pheromone which doesn't affect any ant directly. 

The pheromones are released in the world grid which is a grid storing the amount of pheromones of any type and of food. The player/observer can increase the amount of food in a cell by holding left click and painting the surface.
 
When an ant randomly finds food, it decreases the amount of food in the cell in which it has found it, and now changes in a "carrying food" state. The ant will now follow the cells with the highest amount of "to nest" pheromones. At the same time, it will release "to food" pheromones in the cells it walks on. The nearby ants which have not found food yet will now follow the "to food" pheromone trail, in hopes of finding food.

When an ant brings food to the nest, it switches back in the "not carrying food" state, and tries to find more food.
## Performance
The application is widely and safely multi-threaded in order to get the maximum performance out of the computer running it. One thread handles the rendering, while a fixed number of threads share the update of all the ants evenly (if there are 5 threads and 1000 ants, each thread ticks 200 ants as fast as it can). The amount of threads and ants can be changed in the **settings.ini** file.

Draw calls are minimized. The grid is drawn using one large vertex array, and the ants are drawn using the same principle.

## Usage
Download the latest release [here](https://github.com/glugau/sfml_ant_simulation/releases) and extract the folder inside of the zip archive.
To start the simulation, launch **Ant_Simulation.exe**.
You can tweak some settings in the **settings.ini** file, like the size of the world, the number of ants or the number of threads. A backup file named **default_settings.ini** is also provided, as these are the values I had the best results with.

## Controls
|KEY|ACTION  |
|--|--|
| Left Click | Add food at the location of the cursor. |
| Right Click | Move the camera relative to your cursor position. |
| Mouse Wheel Up/Down | Zoom In/Out |
|GUI Arrows (Top Left of the window) | Speed up / Slow down the simulation |
