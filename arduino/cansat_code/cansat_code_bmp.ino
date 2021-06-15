/////////////////////////////////////////////////////////////////////////////////
// Program to only test BMP 280
/////////////////////////////////////////////////////////////////////////////////

#include <BMP280_DEV.h> 

float temperature, pressure, altitude;         
float AVERAGE_TEMPERATURE, AVERAGE_PRESSURE;
BMP280_DEV bmp280;

void setup() 
{
  Serial.begin(9600);
  if (!bmp280.begin(0x76)) {
    Serial.println(F("BMP280 failed"));
  }
  bmp280.setTimeStandby(TIME_STANDBY_05MS);
  bmp280.startNormalConversion();

  float temperature_sample;
  float pressure_sample;

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
  
  Serial.print(F("Average pressure and temperature: "));
  Serial.print(AVERAGE_PRESSURE); Serial.print(F(","));
  Serial.print(AVERAGE_TEMPERATURE);
  delay(1000);
}

void loop() 
{
  bmp280.getCurrentTempPres(temperature, pressure);
  pressure *= 100;

  altitude = (AVERAGE_TEMPERATURE / -0.0065) * ( pow(pressure / AVERAGE_PRESSURE, 0.19022806) - 1 );

  Serial.print(temperature);  Serial.print(F(","));
  Serial.print(pressure); Serial.print((","));
  Serial.println(altitude);
}
