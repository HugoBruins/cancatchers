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
static const int RXPin = 3, TXPin = 2;
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
String filename = "data";
String dataString;

//this is necessary for calculating altitude based on pressure difference
float gemiddeldeLuchtdruk;
float gemiddeldeTemperatuur;
float constante;
float lapseRate = -0.0065;     //kelvin per meter in the troposphere
int R = 287;                   //gas constant
float g = -9.81;               //gravitational acceleration in the Netherlands
float hoogte;

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
  constante = (1 / (g / (lapseRate * R)));
  for (int i = 0; i <= 100; i++) {
    bmp.readPressure();
    bmp.readTemperature();
  }

  //zero measure (average of 1000 measures of temperature and pressure at starting location)
  for (int i = 0; i <= 999; i++) {
    gemiddeldeLuchtdruk += bmp.readPressure();
    gemiddeldeTemperatuur += bmp.readTemperature();
  }
  gemiddeldeLuchtdruk = gemiddeldeLuchtdruk / 1000;
  gemiddeldeTemperatuur = (gemiddeldeTemperatuur / 1000) + 273.15;
  
  //gives the file inside the sd card reader a random number so other files aren't overwitten
  int randomnumber =  random(10000);
  String nummer = String(randomnumber);
  filename = filename + nummer + ".txt";

  //for debug
  Serial.println(gemiddeldeLuchtdruk);
  Serial.println(gemiddeldeTemperatuur);
  mySerial.println(gemiddeldeLuchtdruk);
  mySerial.println(gemiddeldeTemperatuur);
  Serial.println(filename);
  mySerial.println(filename);

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
  String luchtdruk = String(round(bmp.readPressure()));
  String temperatuur = String(bmp.readTemperature());
  float luchtdruk2 = luchtdruk.toFloat();

  //calculating altitude
  float drukverschil = luchtdruk2 / gemiddeldeLuchtdruk;
  float temperatuurOpHoogte = gemiddeldeTemperatuur * (pow(drukverschil, constante));
  String hoogte = String((temperatuurOpHoogte - gemiddeldeTemperatuur) / lapseRate);

  //adding it all into one string
  dataString = gpsLat + " " + gpsLon + " " + hoogte + " " + temperatuur + " " + luchtdruk;
  
  //writing the data to the sd card 
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  //debug
  Serial.println(dataString);
  
  delay(10);
}
