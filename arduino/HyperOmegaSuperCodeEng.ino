//adding in the libraries

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SD.h>

//mySerial is for the transmitter, ss is for the gps module
TinyGPSPlus gps;
static const int RXPin = 9, TXPin = 10;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial mySerial(6, 5);
String gpsLat = "-";
String gpsLon = "-";


//this is for the bmp280 temperature/pressure sensor
Adafruit_BMP280 bmp; // I2C
const int BMP_address = 0x76;

//chipselect is where the last connection of the sd card reader is connected
const int chipSelect = 4;
String fileName = "data";
String dataString;

//this is necessary for calculating altitude based on pressure difference
float averagePressure;
float averageTemperature;
float constant;
float lapseRate = -0.0065;     //kelvin per meter in the troposphere
int R = 287;                   //gas constant
float g = -9.81;               //gravitational acceleration in the Netherlands
float altitude;

void setup() {
  //starting all the serials at 9600
  Serial.begin(9600);           //arduino serial
  mySerial.begin(9600);         //transmitter serial
  ss.begin(GPSBaud);            //gps serial
  randomSeed(analogRead(1));    //for generating a random number based on unconnected analog noise

  //starting the SD card reader and the bmp280 sensor
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    mySerial.println("sdKaart faalt hem jammer dit");
  }
  bmp.begin(BMP_address);
  
  //preparing the sensor for the zero measure
  constant = (1 / (g / (lapseRate * R)));
  for (int i = 0; i <= 100; i++) {
    bmp.readPressure();
    bmp.readTemperature();
  }

  //zero measure (average of 1000 measures of temperature and pressure at starting location)
  for (int i = 0; i <= 999; i++) {
    averagePressure += bmp.readPressure();
    averageTemperature += bmp.readTemperature();
  }
  averagePressure = averagePressure / 1000;
  averageTemperature = (averageTemperature / 1000) + 273.15;
  
  //gives the file inside the sd card reader a random number so other files aren't overwitten
  int randomNumber =  random(10000);
  String number = String(randomNumber);
  fileName = fileName + number + ".txt";

  //for debug
  Serial.println(averagePressure);
  Serial.println(averageTemperature);
  mySerial.println(averagePressure);
  mySerial.println(averageTemperature);
  Serial.println(fileName);
  mySerial.println(fileName);

  if (!bmp.begin(BMP_address)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    mySerial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  
  //1hz interrupt, this goes every second outside of the main loop to get gps data and to transmit the data
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 16074;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

//code inside 1hz (because of transmitter and gps limits) interrupt loop
ISR(TIMER1_COMPA_vect) {
  //collection of lattitude and longitude from gps module
  //if wires are incorrect, it sends -,-. if there is no satellite connection it sends 0,0 for latt/long. 
  while (ss.available() > 0)
    if (gps.encode(ss.read())) {
      if (gps.location.isValid())
      {
        gpsLat = String((gps.location.lat(), 6));
        gpsLon = String((gps.location.lng(), 6));
      }
      else
      {
        gpsLat = "0";
        gpsLon = "0";
      }
    }
  //the transmitting of the data
  mySerial.println(dataString);
}

//the 100hz normal loop
void loop() {
  //reading pressure and temperature from the sensor
  String pressure = String(round(bmp.readPressure()));
  String temperature = String(bmp.readTemperature());
  float pressure2 = pressure.toFloat();

  //calculating altitude
  float pressureDifference = pressure2 / averagePressure;
  float temperatureAtAltitude = averageTemperature * (pow(pressureDifference, constant));
  String altitude = String((temperatureAtAltitude - averageTemperature) / lapseRate);

  //adding it all into one string
  dataString = gpsLat + " " + gpsLon + " " + altitude + " " + temperature + " " + pressure;
  
  //writing the data to the sd card 
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  //debug
  Serial.println(dataString);
  
  delay(10);
}
