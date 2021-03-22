#include <NMEAGPS.h>
#include <GPSport.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Fat16.h>
#include <Fat16util.h> // use functions to print strings from flash memory

//For SD card stuff
#define CHIP_SELECT     4 // SD chip select pin
SdCard card;
Fat16 file;

//for gps
NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values

//for APC220
NeoSWSerial transmitter(6, 2);

//for temperature/altitude sensor
Adafruit_BMP280 bmp; // I2C

//for calculating altitude based on pressure difference
float averagePressure;
float averageTemperature;
const float constant = (1 / (-9.81 / (-0.0065 * 287)));

//global variables for transmitter
float pressure;
float temperature;
float Altitude;

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))
//------------------------------------------------------------------------------
void error_P(const char* str) {
  PgmPrint("error: ");
  SerialPrintln_P(str);
  if (card.errorCode) {
    PgmPrint("SD error: ");
    Serial.println(card.errorCode, HEX);
  }
  while (false);
}

void setup()
{
  Serial.begin(9600);
  transmitter.begin(9600);

  // initialize the SD card and initialize a FAT16 volume
  if (!card.begin(CHIP_SELECT)) error("card.begin");
  if (!Fat16::init(&card)) error("Fat16::init");

  // create a new file
  char name[] = "LOGGER00.TXT";
  for (uint8_t i = 0; i < 100; i++) {
    name[6] = i / 10 + '0';
    name[7] = i % 10 + '0';
    // O_CREAT - create the file if it does not exist
    // O_EXCL - fail if the file exists
    // O_WRITE - open for write only
    if (file.open(name, O_CREAT | O_EXCL | O_WRITE))break;
  }
  if (!file.isOpen()) error ("create");
  Serial.println(name);

  // clear write error
  file.writeError = false;

  if (file.writeError || !file.sync()) {
    error("write header");
  }

  //transmitter.println(F("Starting initialisation"));

  //have to do this otherwise the transmitter sends garbage
  while (!Serial);

  //warming up sensor (otherwise it acts weird with battery power)
  if (!bmp.begin(0x76)) {
    transmitter.println(F("no BMP280 :("));
  }
  for (int i = 0; i <= 50; i++) {
    bmp.readPressure();
    bmp.readTemperature();
  }

  //getting starting pressure and temperature based on a lot of measurements
  for (int i = 0; i <= 999; i++) {
    averagePressure += bmp.readPressure();
    averageTemperature += bmp.readTemperature();
  }
  averagePressure = averagePressure / 1000;
  averageTemperature = (averageTemperature / 1000) + 273.15;


  //transmitter.println(F("initialisation done"));

  //you have to close the transmitter port for using gps and vice versa.
  transmitter.end();
  gpsPort.begin(9600);

  //transmission of data has to be done at an interval that is slower than 1hz, otherwise the packets get received in a weird manner ?
  //transmitting interrupt (0.95hz)
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  // set compare match register for 0.9500212804766827 Hz increments
  OCR1A = 16646;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();

}

//interrupt code for transmitting
ISR(TIMER1_COMPA_vect) {
  gpsPort.end();
  transmitter.begin(9600);
  while (!Serial);
  transmitter.print(fix.satellites); transmitter.print(F(" "));
  transmitter.print(fix.latitude(), 6); transmitter.print(F(" "));
  transmitter.print(fix.longitude(), 6); transmitter.print(F(" "));
  transmitter.print(Altitude); transmitter.print(F(" "));
  transmitter.print(temperature); transmitter.print(F(" "));
  transmitter.print(pressure, 0); transmitter.print(F(" "));
  transmitter.print(fix.dateTime.hours + 1); transmitter.print(fix.dateTime.minutes); transmitter.println(fix.dateTime.seconds);
  transmitter.end();
  gpsPort.begin(9600);
}


void loop()
{
  getGPS();
}

//this function is basically done in gps quiet time, and gets called once per second.
static void doSomeWork()
{
  pressure = bmp.readPressure();
  temperature = bmp.readTemperature();
  float pressureDifference = pressure / averagePressure;
  float temperatureAltitude = averageTemperature * (pow(pressureDifference, constant));
  Altitude = (temperatureAltitude - averageTemperature) / -0.0065;
  Serial.println(temperature);
  file.print(fix.satellites); file.print(F(" "));
  file.print(fix.latitude(), 6); file.print(F(" "));
  file.print(fix.longitude(), 6); file.print(F(" "));
  file.print(Altitude); file.print(F(" "));
  file.print(temperature); file.print(F(" "));
  file.print(pressure, 0); file.print(F(" "));
  file.println(millis());
  if (!file.sync()) error("sync");
}

//get gps data in a fix structure with neogps.
void getGPS() {
  while (gps.available( gpsPort )) {
    fix = gps.read();
    doSomeWork();
  }
}
