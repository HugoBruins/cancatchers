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

//gps
#include <TinyGPS.h>
TinyGPS gps; // create gps object 
float lat = 28.5458,lon = 77.1703;
SoftwareSerial gpsSerial(3,4);


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
  float temp = bmp.readTemperature();
  long tijd = millis();
  mySerial.print(F("X"));
  mySerial.print(tijd);
  mySerial.print(F("X"));
  mySerial.print(temp);
    
  mySerial.print(F("X"));
  mySerial.print(bmp.readPressure());

  mySerial.print(F("X"));
  mySerial.print(bmp.readAltitude(986.7));
  mySerial.print("X");
  mySerial.println();

  gps.f_get_position(&lat,&lon);
  String latitude = String(lat,6);
  String longitude = String(lon,6);
  Serial.println(latitude+";"+longitude+";"+ (gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites())+";"+temp);
  delay(1000);
}
