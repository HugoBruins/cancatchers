//this cansat code does everything except for writing to the SD card
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
#include <Adafruit_BMP280.h>

LoRa_E32 e32ttl(3, 5, 2, 7, 6);
NMEAGPS  gps;
gps_fix  fix;
Adafruit_BMP280 bmp; // I2C
//all the altitude calculation variables
float AVERAGE_PRESSURE;
float AVERAGE_TEMPERATURE;

//the struct that gets send
struct CansatData {
  float latitude;
  float longitude;
  int sats;
  float temperature;
  float pressure;
  int altitude;
};

void setup()
{
  DEBUG_PORT.begin(9600);
  gpsPort.begin(115200); //we may or may not have overclocked our gps module a bit
  e32ttl.begin();
  while (!Serial);
  delay(100);
  
  //bmp module
  if (!bmp.begin(0x76)) {
    DEBUG_PORT.println(F("BMP280 failed"));
  }
  //warming up the sensor a bit, otherwise battery power seems to interfere.
  for (int i = 0; i <= 50; i++) {
    bmp.readPressure();
    bmp.readTemperature();
    delay(1);
  }
  //getting starting pressure and temperature based on a lot of measurements
  for (int i = 0; i <= 499; i++) {
    AVERAGE_PRESSURE += bmp.readPressure();
    AVERAGE_TEMPERATURE += bmp.readTemperature();
    delay(1);
  }
  AVERAGE_PRESSURE /= 500;
  AVERAGE_TEMPERATURE = (AVERAGE_TEMPERATURE / 500) + 273.15;
}

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
     
    struct CansatData {
      float latitude = fix.latitude();
      float longitude = fix.longitude();
      int sats = fix.satellites;
      float temperature = bmp.readTemperature();
      float pressure = bmp.readPressure();
  
      //-0.0065 is the lapse rate in the troposphere, 0.190163099 is a constant, defined by ((a*R)/g)
      //-153.8461538 is the inverse of the lapse rate (-0.0065), I have a feeling that float multiplication is slightly faster than division
      //the altitude is based on terrain altitude, so there has to be no software adjusting during launch
      int altitude = ( ( AVERAGE_TEMPERATURE * (pow( (bmp.readPressure() / AVERAGE_PRESSURE) , 0.190163099)) ) - (AVERAGE_TEMPERATURE) ) * -153.8461538;

    } CansatData;

    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));
    DEBUG_PORT.println(F(":)"));
  }
}
