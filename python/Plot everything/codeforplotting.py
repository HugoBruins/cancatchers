import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pandas as pd
import numpy as np

gpsfrequency = 1

#gets data
df = pd.read_csv('LOGGER96.txt', sep=" ", header=None)
df.columns = ["sats", "latitude", "longitude", "altitude", "temperature", "pressure", "time"]
mapimg = plt.imread('map.png')

#turns everything in superior numpy arrays for manipulation
arrayLong = np.array(df.longitude.tolist())
arrayLat = np.array(df.latitude.tolist())
plotAlt = np.array(df.altitude.tolist())
plot3dAlt = np.array(df.altitude.tolist())

#getting the boundaries for the 2dimensional map
BBox = ((np.min(arrayLong[np.nonzero(arrayLong)]),   df.longitude.max(),      
          np.min(arrayLat[np.nonzero(arrayLat)]), df.latitude.max()))

#printing it so we can create a map image based on extreme values
print()
print(BBox)

#removing values below zero altitude (sensor noise) and turning gpslatlong values to None type if there is no fix for the 3d map. 
counter = 0
for i in arrayLong:
    if plot3dAlt[counter] <= 0:
        plot3dAlt[counter] = 0
        plotAlt[counter] = 0
    if i == 0:
        arrayLong[counter] = None
        arrayLat[counter] = None
        plot3dAlt[counter] = None
    counter += 1


#this is for calculating wind speed
R = 6373000.0 #radius earth
counter = 0
windspeed = []
gpsintervaltime = gpsfrequency**-1
print("The gps interval set in the python code = ", gpsfrequency, " hz, please check if this is correct")
for i in df.latitude:
    try: 
        lat1 = np.radians(df.latitude[counter])
        lat2 = np.radians(df.latitude[counter+1])
        lon1 = np.radians(df.longitude[counter])
        lon2 = np.radians(df.longitude[counter+1])
        
        dlat = lat2 - lat1
        dlon = lon2 - lon1
        a = np.sin(dlat / 2)**2 + np.cos(lat1) * np.cos(lat2) * np.sin(dlon / 2)**2
        c = 2 * np.arctan2(np.sqrt(a), np.sqrt(1 - a))
        distance = R * c
    except:
        True
    counter += 1
    if distance > 10:
        distance = 0;
    windspeed.append(distance/gpsintervaltime)

#for turning the milliseconds into seconds
timelist = []
for i in df.time.tolist():
    timeval = i / 1000
    timelist.append(timeval)



#for plotting temperature, pressure and wind speed
fig,ax = plt.subplots(4)
ax[0].plot(timelist, df.temperature, color = 'red')
ax[0].set_xlabel('time (s)')
ax[0].set_ylabel('temperature (℃)')
ax[0].grid()
ax[1].plot(timelist, df.pressure, color = 'orange')
ax[1].set_xlabel('time (s)')
ax[1].set_ylabel('pressure (Pa)')
ax[1].grid()
ax[2].plot(timelist, windspeed, color = 'purple')
ax[2].set_xlabel('time (s)')
ax[2].set_ylabel('wind speed (m/s)')
ax[2].grid()
ax[3].plot(timelist, plotAlt, color = 'red')
ax[3].set_xlabel('time (s)')
ax[3].set_ylabel('altitude (m)')
ax[3].grid()

#for plotting  2d map
fig,ax = plt.subplots(figsize = (8,7))
color_map = plt.cm.get_cmap('hot')
im = ax.scatter(arrayLong, arrayLat, cmap=color_map, zorder=1, alpha= 1, c=plot3dAlt, s=10)
fig.colorbar(im, ax=ax, label = "altitude")
ax.set_xlim(BBox[0],BBox[1])
ax.set_ylim(BBox[2],BBox[3])
ax.set_xlabel('longitude')
ax.set_ylabel('latitude')
ax.imshow(mapimg, zorder=0, extent = BBox, aspect= 'equal')  

#for plotting the 3d map
fig = plt.figure()
ax = Axes3D(fig)
ax.set_xlabel('longitude')
ax.set_ylabel('latitude')
ax.set_zlabel('altitude (m)')
plt.plot(arrayLong,arrayLat,plot3dAlt, color = 'hotpink')
plt.show()
