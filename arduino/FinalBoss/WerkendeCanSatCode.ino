/*
   LoRa E32-TTL-100
   Send fixed transmission structured message to a specified point.
   https://www.mischianti.org/2019/12/03/lora-e32-device-for-arduino-esp32-or-esp8266-power-saving-and-sending-structured-data-part-5/

   E32-TTL-100----- Arduino UNO or esp8266
   M0         ----- 3.3v (To config) GND (To send) 7 (To dinamically manage)
   M1         ----- 3.3v (To config) GND (To send) 6 (To dinamically manage)
   TX         ----- RX PIN 2 (PullUP)
   RX         ----- TX PIN 3 (PullUP & Voltage divider)
   AUX        ----- Not connected (5 if you connect)
   VCC        ----- 3.3v/5v
   GND        ----- GND

*/
#include "Arduino.h"
#include "LoRa_E32.h"
#include <NMEAGPS.h>
#include <GPSport.h>

//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(3, 5); // Arduino RX <-- e32 TX, Arduino TX --> e32 RX
LoRa_E32 e32ttl(3, 5, 2, 7, 6);
NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values

void setup()
{
  DEBUG_PORT.begin(9600);
  gpsPort.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  delay(100);
}

struct Message {
  float latitude;
  float longitude;
} message;

int i = 0;

void loop()
{
  //delay(500);
  //Serial.println("Fard1");
  while (gps.available( gpsPort )) {
    fix = gps.read();
    e32ttl.begin();

    i++;
  struct Message {
    float latitude = fix.latitude();
    float longitude = fix.longitude();
  } message;
    
    ResponseStatus rs = e32ttl.sendFixedMessage(0, 3, 4, &message, sizeof(Message));
    DEBUG_PORT.print("fard transmitted");
  }
}
