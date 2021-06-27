## Important

Have some kind of switch or jumper between the TX pin on GPS and the RX or 0 pin on your Arduino, you need to turn this **off** every single time you upload code, and turn this switch **on** when you try the code. Otherwise you might send some random stuff to the GPS module while uploading code, which could make it stop working entirely.

Code with the SD card and GPS will NOT compile if you don't comment all the lines with Serial.print or anything that does anything with serial in `\Arduino\libraries\Fat16\Fat16.cpp`. This has to do with the neoHWserial protocol used with NeoGPS.

## Library dependencies

Install these libraries before trying the code:
- neogps.h
- neohwserial.h
- BMP280_dev.h
- Fat16.h
- lora_32tll.h

## Getting the GPS module working

Please follow the following steps to get the GPS working:

- Make sure there is a switch or something between the TX pin on GPS and RX / 0 pin on the Arduino, this switch needs to be off every time you upload code. The switch needs to be on while running the code however, but you can still use the serial monitor.
- locate the NeoGPS folder inside of the libraries folder of your Arduino software.
- In `.../libraries/neogps/src/GPSport.h` comment whatever was commented and uncomment `//#include <NeoHWSerial.h>`.
- Now run the `NMEAorder` example and take note of the last sentence given trough the debug_port.
- Go to `.../libraries/neogps/src/NMEAGPS_cfg.h` and in `#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GLL`, change `GLL` to whatever your last sentence was in step 3.

## Stability improvements

To increase stability some changes can be made in NeoGPS to decrease dynamic memory. A simple one that instantly decreases dynamic memory is in `GPSfix_cfg.h`, here you can comment all the variables that are not used, i.e. only have `#define GPS_FIX_LOCATION` and `#define GPS_FIX_SATELLITES` uncommented. This will decrease dynamic memory by about 3 to 4%.  

## SD card

For the SD card to work properly, it will need to be below 2gb, and it will have to be formatted to FAT16 (Windows calls this FAT). It can not be a >2gb card that has only 2gb allocated, it strictly has to be <=2gb. 

## Changing GPS settings

In order to change the settings of the GPS module, you will need to unsolder the GPS module and connect it with a USB to TLL adapter (CH340 adapter should work) to your pc. In your PC you can use the program GNSSToolkit3 to change the settings of the GPS module. The settings that we had working perfectly were:

PCAS01 baud rate = 9600 -> 38400 (115200 turns out to be unstable for correctly decoding sentences for the Arduino on battery power)
PCAS02 update rate = 1hz -> 2hz
PCAS04 GPS constellations: GPS (USA), GLONASS (Russia) -> GPS (USA), GLONASS (Russia), BDS (China)

## Extra information

I recommend testing all the codes one by one in the following order:

1. cansat_code_gps.ino
2. cansat_code_gps_sd.ino
3. cansat_code_gps_bmp.ino
4. cansat_code_all.ino
