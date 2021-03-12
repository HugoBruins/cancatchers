//Include de bibliotheken <NMEAGPS.h>, <GPSport.h>, en zorg dat de bibliotheek NeoSWSerial geïnstalleerd is 
// :)
//
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SD.h>
////////////////////////////////////////////////////////////////////////////
#include <NMEAGPS.h>
#include <GPSport.h>
//#include <Streamers.h>
static NMEAGPS  gps;
static gps_fix  fix;

//static void doSomeWork()
//{
//  trace_all( DEBUG_PORT, gps, fix );
//
//}

static void GPSloop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    //doSomeWork();
  }
}
////////////////////////////////////////////////////////////////////////////////
NeoSWSerial mySerial(6, 5);

const int chipSelect = 4;
Adafruit_BMP280 bmp; // I2C
const int BMP_address = 0x76;
String filename = "hoi.txt";

String gpsLat = "-";
String gpsLon = "-";
String dataString;

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

  yeet();

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
  //filename = "hoi.txt";

  //debug

  if (!bmp.begin(BMP_address)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

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

void loop() {
  GPSloop();

  String luchtdruk = String(round(bmp.readPressure()));
  String temperatuur = String(bmp.readTemperature());
  float luchtdruk2 = luchtdruk.toFloat();

  //hoogte berekenen
  float drukverschil = luchtdruk2 / gemiddeldeLuchtdruk;
  float temperatuurOpHoogte = gemiddeldeTemperatuur * (pow(drukverschil, constante));
  String hoogte = String((temperatuurOpHoogte - gemiddeldeTemperatuur) / lapseRate);

  gpsLat = fix.latitude();
  gpsLon = fix.longitude();

  dataString = gpsLat + " " + gpsLon + " " + hoogte + " " + temperatuur + " " + luchtdruk;

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    //print to the serial port too:
  }else{
    Serial.println("oof");
  }

  Serial.println(dataString);
  delay(100);
}

ISR(TIMER1_COMPA_vect) {
  mySerial.println(dataString);
}

void yeet() {
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT);

  DEBUG_PORT.print( F("NMEA.INO: started\n") );
  DEBUG_PORT.print( F("  fix object size = ") );
  DEBUG_PORT.println( sizeof(gps.fix()) );
  DEBUG_PORT.print( F("  gps object size = ") );
  DEBUG_PORT.println( sizeof(gps) );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );

#ifndef NMEAGPS_RECOGNIZE_ALL
#error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
#error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

#if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
      !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
      !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
      !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST )

  DEBUG_PORT.println( F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed.") );

#else
  if (gps.merging == NMEAGPS::NO_MERGING) {
    DEBUG_PORT.print  ( F("\nWARNING: displaying data from ") );
    DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
    DEBUG_PORT.print  ( F(" sentences ONLY, and only if ") );
    DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
    DEBUG_PORT.println( F(" is enabled.\n"
                          "  Other sentences may be parsed, but their data will not be displayed.") );
  }
#endif

  DEBUG_PORT.print  ( F("\nGPS quiet time is assumed to begin after a ") );
  DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
  DEBUG_PORT.println( F(" sentence is received.\n"
                        "  You should confirm this with NMEAorder.ino\n") );

  //trace_header( DEBUG_PORT );
  DEBUG_PORT.flush();

  gpsPort.begin( 9600 );
}
