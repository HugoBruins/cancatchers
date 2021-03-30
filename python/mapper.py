import matplotlib.pyplot as plt
import pandas as pd

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

#for turning the milliseconds into seconds
timelist = []
for i in df.time.tolist():
    timeval = i / 1000
    timelist.append(timeval)

#for plotting temperature and pressure
fig,ax = plt.subplots(2)
ax[0].plot(timelist, df.temperature, color = 'red')
ax[0].set_xlabel('time (s)')
ax[0].set_ylabel('temperature (℃)')
ax[0].grid()
ax[1].plot(timelist, df.pressure, color = 'orange')
ax[1].set_xlabel('time (s)')
ax[1].set_ylabel('pressure (Pa)')
ax[1].grid()

#for plotting the map
fig,ax = plt.subplots(figsize = (8,7))
ax.scatter(df.longitude, df.latitude, zorder=1, alpha= 0.4, c='hotpink', s=10)
ax.set_xlim(BBox[0],BBox[1])
ax.set_ylim(BBox[2],BBox[3])
ax.set_xlabel('longitude')
ax.set_ylabel('latitude')
ax.imshow(mapimg, zorder=0, extent = BBox, aspect= 'equal')  
