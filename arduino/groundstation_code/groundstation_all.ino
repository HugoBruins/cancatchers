//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  connections (Reiceiver module)
//  Module      Arduino
//  M0          4
//  M1          5
//  Rx          2 (This is the MCU Tx lined)
//  Tx          3 (This is the MCU Rx line)
//  Aux         6
//  Vcc         3V3
//  Gnd         Gnd
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <SoftwareSerial.h>
#include <EBYTE.h>

#define PIN_RX 2
#define PIN_TX 7
#define PIN_M0 4
#define PIN_M1 5
#define PIN_AX 6

float previous_time = 0;


// this struct is the same order as on the transmitter
struct DATA {
    uint8_t sats;
    float latitude;
    float longitude;
    int16_t altitude;
    uint16_t temperature;
    uint16_t pressure;
};

// these are just dummy variables, replace with your own
DATA cansat_data;
SoftwareSerial ESerial(PIN_RX, PIN_TX);
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

void setup() {
  Serial.begin(115200);
  ESerial.begin(9600);
  Serial.println(F("Starting Reader"));
  Transceiver.init();
  Transceiver.PrintParameters();
  Serial.println(F(""));
}

void loop() {
  if (ESerial.available()) {

    Transceiver.GetStruct(&cansat_data, sizeof(cansat_data));
    

    //the commas are there so they are easily splitted using python
    // Serial.print(cansat_data.sats); Serial.print(F(","));
    Serial.print(cansat_data.sats); Serial.print(F(","));
    Serial.print(cansat_data.latitude, 6); Serial.print(F(","));
    Serial.print(cansat_data.longitude, 6); Serial.print(F(","));
    Serial.print(cansat_data.altitude); Serial.print(F(","));
    Serial.print(float(cansat_data.temperature) / 100); Serial.print(F(","));
    Serial.print(long(cansat_data.pressure) * 10); Serial.print(F(","));
    Serial.println(millis() - previous_time);
    previous_time = millis();
  }
}
