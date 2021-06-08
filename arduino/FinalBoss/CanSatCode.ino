#include <SoftwareSerial.h>
#include "EBYTE.h"

#define PIN_RX 3
#define PIN_TX 5
#define PIN_M0 7
#define PIN_M1 6
#define PIN_AX 2

struct DATA {
  float longitude; 
  float latitude; 
};

int Chan;
DATA CanSatData;

// you will need to define the pins to create the serial port
SoftwareSerial ESerial(PIN_RX, PIN_TX);

// create the Transmitter object, passing in the serial and pins
EBYTE Transmitter(&ESerial, PIN_M0, PIN_M1, PIN_AX);

void setup() {

  Serial.begin(9600);
  ESerial.begin(9600);

  Serial.println(F("Starting Sender"));

  // this init will set the pinModes for you
  Transmitter.init();

  // all these calls are optional but shown to give examples of what you can do

  Serial.println(Transmitter.GetAirDataRate());
  Serial.println(Transmitter.GetChannel());
}

void loop() {
  CanSatData.longitude = 47.64738937498;
  CanSatData.latitude = 4.77237846837;
  
  Transmitter.SendStruct(&CanSatData, sizeof(CanSatData));

  Serial.print(F("Sending: "));
  delay(1000);
}
