//libraries invoegen

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SD.h>

//mySerial is voor de verzender, ss is voor de gps
TinyGPSPlus gps;
static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial mySerial(6, 5);
String gpsLat = "-";
String gpsLon = "-";


//voor de temperatuursensor
Adafruit_BMP280 bmp; // I2C
const int BMP_address = 0x76;

//chipSelect is voor waar de SD kaart pin is aangesloten
const int chipSelect = 4;
String filename = "data";
String dataString;



//dit is nodig om later het verschil in luchtdruk te meten
float gemiddeldeLuchtdruk;
float gemiddeldeTemperatuur;
float constante;
float lapseRate = -0.0065;     //kelvin per meter in de troposfeer
int R = 287;                   //gasconstante
float g = -9.81;               //zwaartekracht in Nederland
float hoogte;

void setup() {
  //alle serials opstarten
  Serial.begin(9600);           //arduino serial
  mySerial.begin(9600);         //verzender serial
  ss.begin(GPSBaud);            //gps serial
  randomSeed(analogRead(1));    //voor het genereren van een willekeurig nummer (noise van analoge pin)

  //gps opstarten en sensor opstarten
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    mySerial.println("sdKaart faalt hem jammer dit");
  }
  bmp.begin(BMP_address);
  
  //de sensor opwarmen voor de nulmeting
  constante = (1 / (g / (lapseRate * R)));
  for (int i = 0; i <= 100; i++) {
    bmp.readPressure();
    bmp.readTemperature();
  }

  //nulmeting voor temperatuur en luchtdruk bij het starten (gemiddele van 1000 metingen)
  for (int i = 0; i <= 999; i++) {
    gemiddeldeLuchtdruk += bmp.readPressure();
    gemiddeldeTemperatuur += bmp.readTemperature();
  }
  gemiddeldeLuchtdruk = gemiddeldeLuchtdruk / 1000;
  gemiddeldeTemperatuur = (gemiddeldeTemperatuur / 1000) + 273.15;
  
  //geeft het bestand een willekeurig nummer aan het einde
  int randomnumber =  random(10000);
  String nummer = String(randomnumber);
  filename = filename + nummer + ".txt";

  //voor debug en om te kijken of het werkt

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
  
  //de interrupt die elke 1 seconde wordt herhaald buiten de loop om de data te versturen en de gps data te verzamelen
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

//de code binnen de interrupt, dit wordt elke seconde buiten de normale loop herhaald.
ISR(TIMER1_COMPA_vect) {
  //het verzamelen van de gps breedte en hoogtegraad
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
  //het versturen van de data
  mySerial.println(dataString);
}

//de normale loop
void loop() {
  //luchtdruk en temperatuur uitlezen
  String luchtdruk = String(round(bmp.readPressure()));
  String temperatuur = String(bmp.readTemperature());
  float luchtdruk2 = luchtdruk.toFloat();

  //hoogte berekenen
  float drukverschil = luchtdruk2 / gemiddeldeLuchtdruk;
  float temperatuurOpHoogte = gemiddeldeTemperatuur * (pow(drukverschil, constante));
  String hoogte = String((temperatuurOpHoogte - gemiddeldeTemperatuur) / lapseRate);

  //alles samenvoegen
  dataString = gpsLat + " " + gpsLon + " " + hoogte + " " + temperatuur + " " + luchtdruk;
  
  //de data schrijven naar de SD kaart
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  //debug
  Serial.println(dataString);
  
  delay(10);
}
