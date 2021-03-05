/***************************************************************************
  This is a library for the BMP280 humidity, temperature & pressure sensor
  Designed specifically to work with the Adafruit BMEP280 Breakout
  ----> http://www.adafruit.com/products/2651
  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include <SoftwareSerial.h>

SoftwareSerial mySerial(6,5);

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SD.h>
const int chipSelect = 4;

Adafruit_BMP280 bmp; // I2C
const int BMP_address = 0x76;
String filename = "data";

//aerospace engineering variabelen :)

//dit is nodig om later het verschil in luchtdruk te meten
float gemiddeldeLuchtdruk;     
float gemiddeldeTemperatuur;

float constante;
float lapseRate = -0.0065;     //kelvin per meter in de troposfeer
int R = 287;                   //gasconstante
float g = -9.81;            //zwaartekracht volgens SIA

float hoogte;



void setup() {
  randomSeed(analogRead(1));
  Serial.begin(9600);
  
  
  
  
  mySerial.begin(9600);
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    mySerial.println("sdKaart faalt hem jammer dit");
  }
  
  bmp.begin(BMP_address);

  constante = (1 / (g / (lapseRate * R)));
  Serial.println("hoi");

  //1001x de luchtdruk meten en daarvan het gemiddelde nemen, zodat bij één foutieve meting het verschil in hoogte niet zo erg is. 
  for (int i = 0; i <= 1000; i++) {
    gemiddeldeLuchtdruk += bmp.readPressure();
    gemiddeldeTemperatuur += bmp.readTemperature();
  }
  gemiddeldeLuchtdruk = gemiddeldeLuchtdruk / 1001;
  gemiddeldeTemperatuur = (gemiddeldeTemperatuur / 1001)+ 273.15;
  Serial.println(gemiddeldeLuchtdruk);
  Serial.println(gemiddeldeTemperatuur);

  //geeft het bestand een willekeurig nummer aan het einde, zodat elke datalezing uniek is zonder dat er internet nodig is (voor tijd).
  int randomnumber =  random(10000);
  String nummer = String(randomnumber);
  filename = filename + nummer + ".txt";
  Serial.println(filename);


  if (!bmp.begin(BMP_address)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}



void loop() {
  String luchtdruk = String(round(bmp.readPressure()));
  String temperatuur = String(bmp.readTemperature());
  float luchtdruk2 = luchtdruk.toFloat();

  //hoogte berekenen
  float drukverschil = luchtdruk2 / gemiddeldeLuchtdruk;
  float temperatuurOpHoogte = gemiddeldeTemperatuur * (pow(drukverschil,constante));
  String hoogte = String((temperatuurOpHoogte - gemiddeldeTemperatuur) / lapseRate);
  
  String dataString = hoogte + " " + temperatuur + " " + luchtdruk;
 
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
  }
  else {
    //Serial.println("error opening datalog.txt");
  }
  
  Serial.println(dataString);
  mySerial.println(dataString);
  delay(1000);
}
