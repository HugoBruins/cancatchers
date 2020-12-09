/*
 Deze code is voor het versturen van data, dus de satellietcode. 
 
 APC220:
 gnd > gnd
 VCC > 5V
 en > niet aansluiten
 RXD > D10
 TXD > D9
 AUX > niet aansluiten
 SET > niet aansluiten
 
 Temperatuursensor(BMP280):
 VCC -> 3.3V BELANGRIJK! NIET 5V!
 gnd -> gnd
 SCL -> 13
 SDA -> 11
 CSE -> 6
 SDC -> 12
 */
#include <SoftwareSerial.h>
SoftwareSerial mySerial(9,10);
long tijd;

//bmp
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 6

//Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);




void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  //bmp
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
  long tijd = millis();
  Serial.print ("APC220 EPIC :)   ");
  mySerial.print(F("X"));
  mySerial.print(tijd);
  mySerial.print(F("X"));
  mySerial.print(bmp.readTemperature());
    
  mySerial.print(F("X"));
  mySerial.print(bmp.readPressure());

  mySerial.print(F("X"));
  mySerial.print(bmp.readAltitude(986.7));
  mySerial.print("X");
  mySerial.println();
  delay(1000);
}
