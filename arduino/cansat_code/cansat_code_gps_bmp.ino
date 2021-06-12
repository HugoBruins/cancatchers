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
#include <BMP280_DEV.h>  

LoRa_E32 e32ttl(3, 5, 2, 7, 6);
NMEAGPS  gps;
gps_fix  fix;
BMP280_DEV bmp; // I2C
//all the altitude calculation variables
float AVERAGE_PRESSURE;
float AVERAGE_TEMPERATURE;


//the struct that gets send
struct DATA {
  int sats;
  float latitude;
  float longitude;
  float altitude;
  float temperature;
  float pressure;
};

DATA CansatData;

void setup()
{
  long startup_time = millis();
  DEBUG_PORT.begin(9600);
  gpsPort.begin(9600); //we may or may not have overclocked our gps module a bit
  e32ttl.begin();
  while (!Serial);
  delay(100);
  
  //bmp module
  if (!bmp.begin(0x76)) {
    DEBUG_PORT.println(F("BMP280 failed"));
  }

  bmp.setTimeStandby(TIME_STANDBY_05MS);
  bmp.startNormalConversion();

  float temperature_sample;
  float pressure_sample;
  //warming up the sensor a bit, otherwise battery power seems to interfere.
  for (int i = 0; i <= 50; i++) {
    bmp.getCurrentTempPres(temperature_sample, pressure_sample);
  }
  
  //getting starting pressure and temperature based on a lot of measurements
  for (int i = 0; i <= 499; i++) {
    bmp.getCurrentTempPres(temperature_sample, pressure_sample);
    AVERAGE_PRESSURE += temperature_sample;
    AVERAGE_TEMPERATURE += pressure_sample;
  }
  AVERAGE_PRESSURE = (AVERAGE_PRESSURE*100) / 500;
  AVERAGE_TEMPERATURE = (AVERAGE_TEMPERATURE / 500) + 273.15;
  
  DEBUG_PORT.print(F("Average pressure and temperature: "));
  DEBUG_PORT.print(AVERAGE_PRESSURE); DEBUG_PORT.print(F(","));
  DEBUG_PORT.print(AVERAGE_TEMPERATURE); DEBUG_PORT.print(F(","));
  DEBUG_PORT.println(millis() - startup_time);
}

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    int startup_time = millis();

    //get all the data
    CansatData.sats = fix.satellites;
    CansatData.latitude = fix.latitude();
    CansatData.longitude = fix.longitude();
    bmp.getCurrentTempPres(CansatData.temperature, CansatData.pressure);
    CansatData.pressure *= 100; //imagine using hectopascal, kinda weird there library
    CansatData.altitude = ( ( AVERAGE_TEMPERATURE * (pow( (CansatData.pressure / AVERAGE_PRESSURE) , 0.190163099)) ) - (AVERAGE_TEMPERATURE) ) * -153.8461538;
    
    //send the data
    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));
    
    DEBUG_PORT.println(millis() - startup_time);
  }
}
