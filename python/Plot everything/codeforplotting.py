import matplotlib.pyplot as plt
import pandas as pd
import math

#gets data
df = pd.read_csv('LOGGER96.txt', sep=" ", header=None)
df.columns = ["sats", "latitude", "longitude", "altitude", "temperature", "pressure", "time"]
mapimg = plt.imread('map.png')

    

#for easily getting the map image based on the boundaries of the data
arrayLong = [i for i in df.longitude.tolist() if i != 0]
arrayLat = [i for i in df.latitude.tolist() if i != 0]

BBox = ((round(min(arrayLong),8),   df.longitude.max(),      
         round(min(arrayLat),8), df.latitude.max()))
print(BBox)

#this is for calculating wind speed, sometimes coordinates fluctuate a bit too much however
R = 6373000.0 #radius earth
counter = 0
windspeed = []
for i in df.latitude:
    try: 
        lat1 = math.radians(df.latitude[counter])
        lat2 = math.radians(df.latitude[counter+1])
        lon1 = math.radians(df.longitude[counter])
        lon2 = math.radians(df.longitude[counter+1])
        
        dlat = lat2 - lat1
        dlon = lon2 - lon1
        a = math.sin(dlat / 2)**2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2)**2
        c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
        distance = R * c
    except:
        True
    counter += 1
    if distance > 100:
        distance = 0;
    windspeed.append(distance)

#for turning the milliseconds into seconds
timelist = []
for i in df.time.tolist():
    timeval = i / 1000
    timelist.append(timeval)



#for plotting temperature, pressure and wind speed
fig,ax = plt.subplots(3)
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

#for plotting the map
fig,ax = plt.subplots(figsize = (8,7))
ax.scatter(df.longitude, df.latitude, zorder=1, alpha= 0.4, c='hotpink', s=10)
ax.set_xlim(BBox[0],BBox[1])
ax.set_ylim(BBox[2],BBox[3])
ax.set_xlabel('longitude')
ax.set_ylabel('latitude')
ax.imshow(mapimg, zorder=0, extent = BBox, aspect= 'equal')  
