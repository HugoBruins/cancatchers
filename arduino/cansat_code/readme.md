##The libraries you will need:

- neogps.h
- neohwserial.h
- BMP280_dev.h
- Sdfat.h
- lora_32tll.h

##for the GPS to work, you need to configure neogps as followed (go to the neogps library file)

in *.../libraries/neogps/src/GPSport.h* comment whatever was commented and uncomment *//#include <NeoHWSerial.h>*
