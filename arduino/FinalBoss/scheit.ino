#include <NMEAGPS.h>
#include <NeoSWSerial.h>
#include <EBYTE.h>
#include <GPSport.h> //using altsoftserial (pins 8 and 9)

#define PIN_RX 3
#define PIN_TX 5
#define PIN_M0 7
#define PIN_M1 6
#define PIN_AX 2

NMEAGPS  gps;
gps_fix  fix;
NeoSWSerial ESerial(PIN_RX, PIN_TX);
EBYTE Transmitter(&ESerial, PIN_M0, PIN_M1, PIN_AX);

struct DATA {
  float longitude = 0; 
  float latitude = 0; 
};
DATA CanSatData;


void setup()
{
  Serial.begin(9600);
  ESerial.begin(9600);
  gpsPort.begin(9600);
  Transmitter.init();
  Transmitter.PrintParameters();
}

//--------------------------

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    
    //GPS quiet time
    delay(10);
    CanSatData.longitude++; 
    while(!Serial);
    Transmitter.SendStruct(&CanSatData, sizeof(CanSatData));
    Serial.print("Sending: "); Serial.println(CanSatData.longitude);
    delay(10);
  }
}

