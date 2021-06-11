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

LoRa_E32 e32ttl(3, 5, 2, 7, 6);
NMEAGPS  gps;
gps_fix  fix;

//the struct that gets send
struct CansatData {
  float latitude;
  float longitude;
  int sats;
};

void setup()
{
  DEBUG_PORT.begin(9600);
  gpsPort.begin(115200); //we may or may not have overclocked our gps module a bit
  e32ttl.begin();
  while (!Serial);
  delay(100);
}

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
     
    struct CansatData {
      float latitude = fix.latitude();
      float longitude = fix.longitude();
      int sats = fix.satellites;
    } CansatData;

    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));
    DEBUG_PORT.println(F(":)"));
  }
}
