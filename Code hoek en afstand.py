import math

R = 6373000.0     #straal van de Aarde in meters

#stel mijn huis:
lat1 = math.radians(51.8219176)
lon1 = math.radians(4.7902939)
hoogte1 = 10

#stel vliegtuig boven gomarus

lat2 = math.radians(51.8378278)
lon2 = math.radians(4.972836951385773)
hoogte2 = 1000


#berekent afstand

dlon = lon2 - lon1

dlat = lat2 - lat1

a = math.sin(dlat / 2)**2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2)**2

c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
distance = R * c

#berekent de richting die het grondstation moet opwijzen in graden van kompas

angle = math.atan2(math.sin(dlon)*math.cos(lat2), math.cos(lat1)*math.sin(lat2) - math.sin(lat1)*math.cos(lat2)*math.cos(dlon))
angle = ((angle * 180/math.pi)+360) % 360

#berekent de hoek voor hoogte

dhoogte = hoogte2 - hoogte1
angleHoogte = math.tanh(dhoogte / distance) * 180/math.pi


print("hoekXY kompas: ",angle)
print("hoekhoogte",angleHoogte)
print("afstand in meters: ",distance)



