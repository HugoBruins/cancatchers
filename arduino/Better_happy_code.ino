#include <NMEAGPS.h>
#include <GPSport.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>


NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values
NeoSWSerial transmitter(6, 5);
Adafruit_BMP280 bmp; // I2C
const int BMP_address = 0x76;

String gpsLat = "-";
String gpsLon = "-";
String dataString;

float averagePressure;
float averageTemperature;
float constant;
float Altitude;
long bootTime;


void setup()
{

  Serial.begin(9600);
  transmitter.begin(9600);
  while (!Serial);
  transmitter.println();
  transmitter.println("Ground control to major Tom");
  Serial.print("Hello!");

  bmp.begin(BMP_address);
  constant = (1 / (-9.81 / (-0.0065 * 287)));
  
  //warming up sensor (otherwise it acts weird with battery power)
  for (int i = 0; i <= 50; i++) {
    bmp.readPressure();
    bmp.readTemperature();
  }
  
  for (int i = 0; i <= 999; i++) {
    averagePressure += bmp.readPressure();
    averageTemperature += bmp.readTemperature();
  }

  averagePressure = averagePressure / 1000;
  averageTemperature = (averageTemperature / 1000) + 273.15;

  transmitter.println("Take your protein pills and put your helmet on!");

  if (!bmp.begin(BMP_address)) {
    transmitter.println(F("no BMP280 :("));
  }
  transmitter.end();

  

  gpsPort.begin(9600);

  //transmitting interrupt (0.95hz)
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  // set compare match register for 0.9500212804766827 Hz increments
  OCR1A = 16446;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();

  bootTime = millis();
}

//interrupt code for transmitting
ISR(TIMER1_COMPA_vect) {
  gpsPort.end();
  transmitter.begin(9600);
  while (!Serial)
    ;
  transmitter.println(dataString);
  transmitter.end();
  gpsPort.begin(9600);
}


void loop()
{
  getGPS();

  float pressure = round(bmp.readPressure());
  float temperature = bmp.readTemperature();
  float pressureDifference = pressure / averagePressure;
  float temperatureAltitude = averageTemperature * (pow(pressureDifference, constant));
  Altitude = (temperatureAltitude - averageTemperature) / -0.0065;
  long Time = millis() - bootTime;

  dataString = gpsLat + " " + gpsLon + " " + String(Altitude) + " " + String(temperature) + " " + String(pressure, 0) + " " + String(Time);
  
}

void getGPS() {
  while (gps.available( gpsPort )) {
    fix = gps.read();
    gpsLat = String(fix.latitude(), 6 );
    gpsLon = String(fix.longitude(), 6);
  }
}
