#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SD.h>

SoftwareSerial mySerial(6, 5);
TinyGPSPlus gps;

static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);

const int chipSelect = 4;
Adafruit_BMP280 bmp; // I2C
const int BMP_address = 0x76;
String filename = "data";

String gpsLat = "-";
String gpsLon = "-";

//dit is nodig om later het verschil in luchtdruk te meten
float gemiddeldeLuchtdruk;
float gemiddeldeTemperatuur;

float constante;
float lapseRate = -0.0065;     //kelvin per meter in de troposfeer
int R = 287;                   //gasconstante
float g = -9.81;               //zwaartekracht volgens SIA
float hoogte;



void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  ss.begin(GPSBaud);
  randomSeed(analogRead(1));


  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    mySerial.println("sdKaart faalt hem jammer dit");
  }

  bmp.begin(BMP_address);

  constante = (1 / (g / (lapseRate * R)));

  //de sensor warmdraaien voor de nulmeting
  for (int i = 0; i <= 100; i++) {
    bmp.readPressure();
    bmp.readTemperature();
  }

  //nulmeting
  for (int i = 0; i <= 999; i++) {
    gemiddeldeLuchtdruk += bmp.readPressure();
    gemiddeldeTemperatuur += bmp.readTemperature();
  }

  gemiddeldeLuchtdruk = gemiddeldeLuchtdruk / 1000;
  gemiddeldeTemperatuur = (gemiddeldeTemperatuur / 1000) + 273.15;


  //geeft het bestand een willekeurig nummer aan het einde, zodat elke datalezing uniek is zonder dat er internet nodig is (voor tijd).
  int randomnumber =  random(10000);
  String nummer = String(randomnumber);
  filename = filename + nummer + ".txt";

  //debug

  Serial.println(gemiddeldeLuchtdruk);
  Serial.println(gemiddeldeTemperatuur);
  mySerial.println(gemiddeldeLuchtdruk);
  mySerial.println(gemiddeldeTemperatuur);
  Serial.println(filename);
  mySerial.println(filename);


  if (!bmp.begin(BMP_address)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}



void loop() {
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

  if (millis() > 10000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }

  String luchtdruk = String(round(bmp.readPressure()));
  String temperatuur = String(bmp.readTemperature());
  float luchtdruk2 = luchtdruk.toFloat();

  //hoogte berekenen
  float drukverschil = luchtdruk2 / gemiddeldeLuchtdruk;
  float temperatuurOpHoogte = gemiddeldeTemperatuur * (pow(drukverschil, constante));
  String hoogte = String((temperatuurOpHoogte - gemiddeldeTemperatuur) / lapseRate);

  String dataString = gpsLat + " " + gpsLon + " " + hoogte + " " + temperatuur + " " + luchtdruk;

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
  }

  Serial.println(dataString);
  mySerial.println(dataString);
  delay(1000);
}
