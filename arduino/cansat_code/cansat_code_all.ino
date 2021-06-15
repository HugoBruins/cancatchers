/////////////////////////////////////////////////////////////////////////////////
// BMP + GPS + Transmission + SD card
// 68% Dynamic memory! 💪
/////////////////////////////////////////////////////////////////////////////////
#include <NMEAGPS.h>
#include <BMP280_DEV.h> 
#include <LoRa_E32.h>
#include <GPSport.h>
#include <SdFat.h>
#include <SPI.h>
    
float AVERAGE_TEMPERATURE, AVERAGE_PRESSURE;
float temperature_sample, pressure_sample;
long last_time;
const int chipSelect = 4;

BMP280_DEV bmp280;
NMEAGPS  gps;
LoRa_E32 e32ttl(3, 5, 2, 7, 6);
SdFat SD;

gps_fix  fix;
File logfile;

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
  delay(100);
  initSD();
  delay(200);
}

void loop() 
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    static uint16_t lastLoggingTime  = 0;
    uint16_t startLoggingTime = millis();
    
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

    logfile.print(CansatData.sats); logfile.print(F(","));
    logfile.print(CansatData.latitude,6); logfile.print(F(","));
    logfile.print(CansatData.longitude,6); logfile.print(F(","));
    logfile.print(CansatData.altitude,1); logfile.print(F(","));
    logfile.print(CansatData.temperature,2); logfile.print(F(","));
    logfile.print(CansatData.pressure,1); logfile.print(F(","));
    logfile.println(millis());
    
    static uint16_t lastFlushTime = 0;
    if (startLoggingTime - lastFlushTime > 1000) {
        lastFlushTime = startLoggingTime;
        logfile.flush();
    }
    lastLoggingTime = (uint16_t) millis() - startLoggingTime;
    
    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));
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
