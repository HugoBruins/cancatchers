/////////////////////////////////////////////////////////////////////////////////
// BMP + GPS + Transmission + SD card.
// 64% Dynamic memory!
// This code will still work without a connected GPS module.
// But will also send GPS if the GPS is connected properly.
// Just in case something disconnects at launch.
// The module also transmits slightly faster in this case, because no 2x 4 byte floats are send. 
/////////////////////////////////////////////////////////////////////////////////
#include <NMEAGPS.h>
#include <BMP280_DEV.h> 
#include <LoRa_E32.h>
#include <GPSport.h>
#include <Fat16.h>
    
float AVERAGE_TEMPERATURE, AVERAGE_PRESSURE;
float temperature_sample, pressure_sample;
long last_time;
const uint8_t CHIP_SELECT = 4;
static uint32_t last_rx = 0UL;
static uint32_t baudStartTime = 0UL;

bool gps_error = false;
uint32_t previous_gps_check_time = millis();

BMP280_DEV bmp280;
LoRa_E32 e32ttl(3, 5, 2, 7, 6);
NMEAGPS  gps;
SdCard card;

Fat16 file;
gps_fix  fix;


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
  DEBUG_PORT.println(AVERAGE_TEMPERATURE);
  delay(100);

  if (!card.begin(CHIP_SELECT)) DEBUG_PORT.println(F("Oh no your card"));
  Fat16::init(&card);

  // create a new file, with a unique number
  char name[] = "LOGGER00.TXT";
  for (uint8_t i = 0; i < 100; i++) {
    name[6] = i / 10 + '0';
    name[7] = i % 10 + '0';
    if (file.open(name, O_CREAT | O_EXCL | O_WRITE))break;
  }
  delay(200);
}

void loop()
{
  // if gps_error = false, i.e. after startup. 
  if (!gps_error) {
    // this is a loop that goes every 3 seconds
    // if the gps works, it should keep gps_error false
    // if nothing was received in 5 seconds, it will turn gps_error true
    uint32_t current_gps_check_time = millis();
    if (current_gps_check_time - previous_gps_check_time >= 3000) {
      previous_gps_check_time = current_gps_check_time;
      check_gps();
    }
    
    // The general gps loop as we know it.
    while (gps.available( gpsPort )) {
      fix = gps.read();
      last_rx = millis();
      
      bmp280.getCurrentTempPres(temperature_sample, pressure_sample);
      pressure_sample *= 100;
  
      struct CansatData {
        uint8_t sats = fix.satellites;
        float latitude = fix.latitude();
        float longitude = fix.longitude();
        int16_t altitude = (AVERAGE_TEMPERATURE / -0.0065) * ( pow(pressure_sample / AVERAGE_PRESSURE, 0.19022806) - 1 );
        uint16_t temperature = temperature_sample * 100;
        uint16_t pressure = pressure_sample / 10;
      } CansatData;
  
      ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));
  
      file.print(CansatData.sats); file.print(F(","));
      file.print(CansatData.latitude, 6); file.print(F(","));
      file.print(CansatData.longitude, 6); file.print(F(","));
      file.print(CansatData.altitude); file.print(F(","));
      file.print(CansatData.temperature); file.print(F(","));
      file.print(CansatData.pressure); file.print(F(","));
      file.println(millis());
      file.sync();
  
      DEBUG_PORT.println(F("hallo"));
    } 
  }
  //if the GPS module disconnected
  else {
    bmp280.getCurrentTempPres(temperature_sample, pressure_sample);
    pressure_sample *= 100;

    //we can see on the receiver module that the gps is not working by receiving 99
    struct CansatData {
      uint8_t sats = 99;
      int16_t altitude = (AVERAGE_TEMPERATURE / -0.0065) * ( pow(pressure_sample / AVERAGE_PRESSURE, 0.19022806) - 1 );
      uint16_t temperature = temperature_sample * 100;
      uint16_t pressure = pressure_sample / 10;
    } CansatData;

    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &CansatData, sizeof(CansatData));

    file.print(CansatData.sats); file.print(F(","));
    file.print(CansatData.altitude); file.print(F(","));
    file.print(CansatData.temperature); file.print(F(","));
    file.print(CansatData.pressure); file.print(F(","));
    file.println(millis());
    file.sync();
  }
}

static void check_gps()
{
  uint32_t current_ms         = millis();
  uint32_t ms_since_last_rx   = current_ms - last_rx;

  if (ms_since_last_rx > 5000) {
    DEBUG_PORT.println(F("GPS is not connected / working"));
    gps_error = true;
  }
}
