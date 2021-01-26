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

SoftwareSerial mySerial(10, 9);

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SD.h>
const int chipSelect = 4;

Adafruit_BMP280 bmp; // I2C
const int BMP_address = 0x76;
String filename = "data";


void setup() {
  randomSeed(analogRead(1));
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
  }
  Serial.println("card initialized.");
  bmp.begin(BMP_address);

  int randomnumber =  random(10000);
  String nummer = String(randomnumber);
  Serial.println(nummer);
  filename = filename + nummer + ".txt";
  Serial.println(filename);


  if (!bmp.begin(BMP_address)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}



void loop() {

  String temperatuur = String(bmp.readTemperature());
  String luchtdruk = String(round(bmp.readPressure()));
  String dataString = temperatuur + " " + luchtdruk;
 
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
  }
  else {
    Serial.println("error opening datalog.txt");
  }
  
  Serial.println(dataString);
  mySerial.print(temperatuur + " " + luchtdruk);
  delay(1000);
}
