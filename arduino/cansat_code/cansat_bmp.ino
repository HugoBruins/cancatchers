/////////////////////////////////////////////////////////////////////////////////
// Program to only test BMP 280
/////////////////////////////////////////////////////////////////////////////////

#include <BMP280_DEV.h>                           // Include the BMP280_DEV.h library

float temperature, pressure, altitude;            // Create the temperature, pressure and altitude variables
float AVERAGE_TEMPERATURE, AVERAGE_PRESSURE;
BMP280_DEV bmp280;                                // Instantiate (create) a BMP280_DEV object and set-up for I2C operation (address 0x77)

void setup() 
{
  Serial.begin(9600);                           // Initialise the serial port
  if (!bmp280.begin(0x76)) {
    Serial.println(F("BMP280 failed"));
  }
  //bmp280.setPresOversampling(OVERSAMPLING_X4);    // Set the pressure oversampling to X4
  //bmp280.setTempOversampling(OVERSAMPLING_X1);    // Set the temperature oversampling to X1
  //bmp280.setIIRFilter(IIR_FILTER_4);              // Set the IIR filter to setting 4
  bmp280.setTimeStandby(TIME_STANDBY_05MS);     // Set the standby time to 2 seconds
  bmp280.startNormalConversion();                 // Start BMP280 continuous conversion in NORMAL_MODE  

  float temperature_sample;
  float pressure_sample;
  //warming up the sensor a bit, otherwise battery power seems to interfere.
  for (int i = 0; i <= 50; i++) {
    bmp280.getCurrentTempPres(temperature_sample, pressure_sample);
  }

  //getting starting pressure and temperature based on a lot of measurements
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
  bmp280.getCurrentTempPres(temperature, pressure);    // Check if the measurement is complete
  pressure *= 100;

  altitude = (AVERAGE_TEMPERATURE / -0.0065) * ( pow(pressure / AVERAGE_PRESSURE, 0.19022806) - 1 );

  // Serial.print(AVERAGE_TEMPERATURE); Serial.print(F(",")); Serial.print(C2); Serial.print(F(",")); Serial.println(C3);

  Serial.print(temperature);  Serial.print(F(","));                  // Display the results    
  Serial.print(pressure); Serial.print((","));
  Serial.println(altitude);
}
