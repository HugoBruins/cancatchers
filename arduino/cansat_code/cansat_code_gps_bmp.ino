/////////////////////////////////////////////////////////////////////////////////
// BMP + GPS + Transmission
/////////////////////////////////////////////////////////////////////////////////
#include <NMEAGPS.h>
#include <BMP280_DEV.h> 
#include <LoRa_E32.h>
#include <GPSport.h>
    
float AVERAGE_TEMPERATURE, AVERAGE_PRESSURE;
float temperature_sample, pressure_sample;
long last_time;

BMP280_DEV bmp280;
NMEAGPS  gps;
gps_fix  fix;
LoRa_E32 e32ttl(3, 5, 2, 7, 6);

struct CansatData {
  int sats = 13;
  float latitude;
  float longitude;
  int altitude;
  int temperature;
  int pressure;
  long time;
};

void setup() 
{
  DEBUG_PORT.begin(9600);
  e32ttl.begin();
  gpsPort.begin(9600);
  delay(100);
  
  if (!bmp280.begin(0x76)) {
    DEBUG_PORT.println(F("BMP280 failed"));
  }
  bmp280.setTimeStandby(TIME_STANDBY_05MS);
  bmp280.startNormalConversion();

  for (int i = 0; i <= 50; i++) {
    bmp280.getCurrentTempPres(temperature_sample, pressure_sample);
  }

  for (int i = 0; i <= 499; i++) {
    bmp280.getCurrentTempPres(temperature_sample, pressure_sample);
    AVERAGE_PRESSURE += pressure_sample * 100;
    AVERAGE_TEMPERATURE += temperature_sample + 273.15;
  }
  AVERAGE_PRESSURE = AVERAGE_PRESSURE / 500;
  AVERAGE_TEMPERATURE = AVERAGE_TEMPERATURE / 500;
  
  DEBUG_PORT.print(F("Average pressure and temperature: "));
  DEBUG_PORT.print(AVERAGE_PRESSURE); DEBUG_PORT.print(F(","));
  DEBUG_PORT.print(AVERAGE_TEMPERATURE);
  delay(1000);
}

void loop() 
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    bmp280.getCurrentTempPres(temperature_sample, pressure_sample);
    pressure_sample *= 100;

    
    struct CansatData {
      int sats = fix.satellites;
      float latitude = fix.latitude();
      float longitude = fix.longitude();
      int altitude = (AVERAGE_TEMPERATURE / -0.0065) * ( pow(pressure_sample / AVERAGE_PRESSURE, 0.19022806) - 1 );
      int temperature = temperature_sample * 100;
      int pressure = pressure_sample / 10;
    } CansatData;

    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));
  }
}
