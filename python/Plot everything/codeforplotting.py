import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pandas as pd
import numpy as np

#change this if necessary!
seperation_mark = ' '       #say you use commas between each send variable, you would make this ','

#gets data
df = pd.read_csv('LOGGER96.txt', sep= seperation_mark, header=None)
df.columns = ["sats", "latitude", "longitude", "altitude", "temperature", "pressure", "time"]
map_img = plt.imread('map.png')

#turns everything in superior numpy arrays for manipulation
array_long = np.array(df.longitude.tolist())
array_lat = np.array(df.latitude.tolist())
plot_alt = np.array(df.altitude.tolist())
plot_3d_alt = np.array(df.altitude.tolist())

#getting the boundaries for the 2dimensional map
boundary_box = ((np.min(array_long[np.nonzero(array_long)]),   df.longitude.max(),      
          np.min(array_lat[np.nonzero(array_lat)]), df.latitude.max()))

#printing it so we can create a map image based on extreme values
print()
print(boundary_box)

#removing values below zero altitude (sensor noise) and turning gpslatlong values to None type if there is no fix for the 3d map. 
for counter, longitude in enumerate(array_long):
    if plot_3d_alt[counter] <= 0:
        plot_3d_alt[counter] = 0
        plot_alt[counter] = 0
    if longitude == 0:
        array_long[counter] = None
        array_lat[counter] = None
        plot_3d_alt[counter] = None

#this is for calculating wind speed
radius_earth = 6373000.0 #radius earth
wind_speed = []
for counter, i in enumerate(df.latitude):
    try: 
        lat1 = np.radians(df.latitude[counter])
        lat2 = np.radians(df.latitude[counter+1])
        lon1 = np.radians(df.longitude[counter])
        lon2 = np.radians(df.longitude[counter+1])
        
        dlat = lat2 - lat1
        dlon = lon2 - lon1
        a = np.sin(dlat / 2) ** 2 + np.cos( lat1 ) * np.cos( lat2 ) * np.sin( dlon / 2 )**2
        c = 2 * np.arctan2(np.sqrt(a), np.sqrt(1 - a))
        distance = radius_earth * c
        
        time_difference = ( df.time[counter+1] - df.time[counter] ) / 1000
    except:
        True
    if distance > 10:
        distance = 0;
    
    wind_speed.append(distance / time_difference)

#for turning the milliseconds into seconds
timelist = []
for i in df.time.tolist():
    timeval = i / 1000
    timelist.append(timeval)

#for plotting temperature, pressure and wind speed
fig, ax = plt.subplots(4)
ax[0].plot(timelist, df.temperature, color = 'red')
ax[0].set_xlabel('time (s)')
ax[0].set_ylabel('temperature (℃)')
ax[0].grid()
ax[1].plot(timelist, df.pressure, color = 'orange')
ax[1].set_xlabel('time (s)')
ax[1].set_ylabel('pressure (Pa)')
ax[1].grid()
ax[2].plot(timelist, wind_speed, color = 'purple')
ax[2].set_xlabel('time (s)')
ax[2].set_ylabel('wind speed (m/s)')
ax[2].grid()
ax[3].plot(timelist, plot_alt, color = 'red')
ax[3].set_xlabel('time (s)')
ax[3].set_ylabel('altitude (m)')
ax[3].grid()

#for plotting  2d map
fig,ax = plt.subplots(figsize = (8,7))
color_map = plt.cm.get_cmap('hot')
im = ax.scatter(array_long, array_lat, cmap=color_map, zorder=1, alpha= 1, c=plot_3d_alt, s=10)
fig.colorbar(im, ax=ax, label = "altitude (m)")
ax.set_xlim(boundary_box[0],boundary_box[1])
ax.set_ylim(boundary_box[2],boundary_box[3])
ax.set_xlabel('longitude')
ax.set_ylabel('latitude')
ax.imshow(map_img, zorder=0, extent = boundary_box, aspect= 'equal')  

#for plotting the 3d map
fig = plt.figure()
ax = Axes3D(fig)
ax.set_xlabel('longitude')
ax.set_ylabel('latitude')
ax.set_zlabel('altitude (m)')
plt.plot(array_long,array_lat,plot_3d_alt, color = 'hotpink')
plt.show()
