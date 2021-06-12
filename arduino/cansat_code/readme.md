## The libraries you will need:

- neogps.h
- neohwserial.h
- BMP280_dev.h
- Sdfat.h
- lora_32tll.h

## for the GPS to work, you need to configure neogps

- make sure there is a switch or something between the pin TX on GPS and RX / 0 pin on the Arduino, this switch needs to be off every time you upload code.
- in ._../libraries/neogps/src/GPSport.h_ comment whatever was commented and uncomment _//#include <NeoHWSerial.h>_
- now run the NMEAorder example and take note of the last sentence given trough the debug_port
- go to .../libraries/neogps/src/NMEAGPS_cfg.h and change _#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA__**GLL**, change GLL to whatever your last sentence was in step 3.
- Now it should work!
