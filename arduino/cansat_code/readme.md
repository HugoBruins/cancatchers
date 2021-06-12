## The libraries you will need:

- neogps.h
- neohwserial.h
- BMP280_dev.h
- Sdfat.h
- lora_32tll.h

## For the GPS to work, you need to configure neogps

- Make sure there is a switch or something between the TX pin on GPS and RX / 0 pin on the Arduino, this switch needs to be off every time you upload code. The switch needs to be on while running the code however, but you can still use the serial monitor.
- In ._../libraries/neogps/src/GPSport.h_ comment whatever was commented and uncomment _//#include <NeoHWSerial.h>_.
- Now run the NMEAorder example and take note of the last sentence given trough the debug_port.
- Go to .../libraries/neogps/src/NMEAGPS_cfg.h and in _#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA__**GLL**, change **GLL** to whatever your last sentence was in step 3.
