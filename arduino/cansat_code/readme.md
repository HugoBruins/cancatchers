## The libraries you will need:

- neogps.h
- neohwserial.h
- BMP280_dev.h
- Sdfat.h
- lora_32tll.h

## for the GPS to work, you need to configure neogps as followed (go to the neogps library file)

in ._../libraries/neogps/src/GPSport.h_ comment whatever was commented and uncomment _//#include <NeoHWSerial.h>_
