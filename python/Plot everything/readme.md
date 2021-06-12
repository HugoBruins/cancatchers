# Information

The code makes 3 different plots as of now, one has all the data graphs of altitude, pressure, temperature, wind / horizontal speed. The other graph is a 2d map with all the latitude and longitude information plotted and the third graph is a 3 dimensional graph that has latitude and longitude, but also altitude information plotted, sadly I cannot get a nice image of the map for the 3d graph. 

The data from these pictures is taken from a caveman distance test with our old transceiver modules. 

## How to get the code working

Make sure you have a nice python IDE installed, and install the following libraries for python using pip:

- matplotlib
- pandas
- numpy

Now drop the code into a folder, and also drop LOGGER96.txt and map.png in that same folder, and run the code, voila, it works! (hopefully)

To get your own map image, run the code once with your own data file, and in the console it will print a boundary box, these are the maximum / minumum longitudes and latitudes, now go to openstreetmap.org and fill in these values in the box over at the left side. Now go to the top right corner and press the 'share' button. Here check the box 'modified picture dimensions', and manually drag the corners of the new appearing box over the old one that fits the boundaries, pick a nice scale that gives good image resolution and download the png, change the name of this png to map and drop it into the same folder as the code. 


## Graphs:

### The temperature / pressure / wind speed / altitude plot
![data graphs](https://user-images.githubusercontent.com/25268098/121771673-032dac80-cb71-11eb-835e-5add11a95b53.png)

### The 2d map plot
![2d map](https://user-images.githubusercontent.com/25268098/121771681-10e33200-cb71-11eb-9bb5-a613e46859bd.png)

### The 3d map plot
![3d map](https://user-images.githubusercontent.com/25268098/121771595-94505380-cb70-11eb-9d5d-ccbaeb07c86f.png)
