// This code features all the cansat code except for BMP
/*
   I will let Thomas handle all the pins here :)
   
   E32-TTL-100----- Arduino UNO or esp8266
   M0         ----- 3.3v (To config) GND (To send) 7 (To dinamically manage)
   M1         ----- 3.3v (To config) GND (To send) 6 (To dinamically manage)
   TX         ----- RX PIN 2 (PullUP)
   RX         ----- TX PIN 3 (PullUP & Voltage divider)
   AUX        ----- Not connected (5 if you connect)
   VCC        ----- 3.3v/5v
   GND        ----- GND
*/

#include <Arduino.h>
#include <LoRa_E32.h>
#include <NMEAGPS.h>
#include <GPSport.h>
#include <SdFat.h>
#include <SPI.h>

LoRa_E32 e32ttl(3, 5, 2, 7, 6);
NMEAGPS  gps;
gps_fix  fix;


//the struct that gets send
struct CansatData {
  int sats;
  float latitude;
  float longitude;
};

//sd card using sdFAT
SdFat SD;
const int chipSelect = 4;
File logfile;

void setup()
{
  DEBUG_PORT.begin(9600);
  gpsPort.begin(115200); //we may or may not have overclocked our gps module a bit
  e32ttl.begin();
  while (!DEBUG_PORT);
  delay(100);

  //sd card
  initSD();
}

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    
    static uint16_t lastLoggingTime  = 0;
    uint16_t startLoggingTime = millis();
    struct CansatData {
      float latitude = fix.latitude();
      float longitude = fix.longitude();
      int sats = fix.satellites; 
    } CansatData;

    logfile.print(CansatData.latitude,6); logfile.print(F(","));
    logfile.print(CansatData.longitude,6); logfile.print(F(","));
    logfile.print(CansatData.sats); logfile.print(F(","));
    logfile.println(millis());

    static uint16_t lastFlushTime = 0;
    if (startLoggingTime - lastFlushTime > 1000) {
        lastFlushTime = startLoggingTime;
        logfile.flush();
    }
    lastLoggingTime = (uint16_t) millis() - startLoggingTime;
    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));
    
    DEBUG_PORT.println(F(":)"));
  }
}

void initSD()
{
  DEBUG_PORT.println( F("Initializing SD card...") );
  if (!SD.begin(chipSelect)) {
    DEBUG_PORT.println( F("  SD card failed, or not present") );
  }
  DEBUG_PORT.println( F("  SD card initialized.") );
  
  // Pick a numbered filename, 00 to 99.
  char filename[15] = "data_##.txt";
  for (uint8_t i=0; i<100; i++) {
    filename[5] = '0' + i/10;
    filename[6] = '0' + i%10;
    if (!SD.exists(filename)) {
      break;
      }
    }

  logfile = SD.open(filename, FILE_WRITE);
  if (!logfile) {
    DEBUG_PORT.print( F("Couldn't create ") ); 
      DEBUG_PORT.println(filename);
  }

  DEBUG_PORT.print( F("Writing to ") ); 
  DEBUG_PORT.println(filename);
}
