//this code works with the canswat_all_fallback code

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
#define PIN_TX 3
#define PIN_M0 4
#define PIN_M1 5
#define PIN_AX 6

float previous_time = 0;
bool gps_error = false;


// this struct is the same order as on the transmitter

// these are just dummy variables, replace with your own

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

  if (gps_error == false) {
      if (ESerial.available()) {
        struct DATA {
        uint8_t sats;
        float latitude;
        float longitude;
        int16_t altitude;
        uint16_t temperature;
        uint16_t pressure;
      };
      DATA cansat_data;
      
      Transceiver.GetStruct(&cansat_data, sizeof(cansat_data));
      
      if (cansat_data.sats == 99) {
        gps_error = true;
      }

      //the commas are there so they are easily splitted using python
      // Serial.print(cansat_data.sats); Serial.print(F(","));
      Serial.print(cansat_data.sats); Serial.print(F(","));
      Serial.print(cansat_data.latitude, 6); Serial.print(F(","));
      Serial.print(cansat_data.longitude, 6); Serial.print(F(","));
      Serial.print(cansat_data.altitude); Serial.print(F(","));
      Serial.print(float(cansat_data.temperature) / 100); Serial.print(F(","));
      Serial.print(long(cansat_data.pressure) * 10); Serial.print(F(","));
      Serial.print(millis()); Serial.print(F(","));
      Serial.println(millis() - previous_time);
      previous_time = millis();
    }
  }
  else {
    if (ESerial.available()) {
        struct DATA {
          uint8_t sats;
          int16_t altitude;
          uint16_t temperature;
          uint16_t pressure;
        };
        DATA cansat_data;
        
        Transceiver.GetStruct(&cansat_data, sizeof(cansat_data));

        //the commas are there so they are easily splitted using python
        // Serial.print(cansat_data.sats); Serial.print(F(","));
        Serial.print(cansat_data.sats); Serial.print(F(","));
        Serial.print(cansat_data.altitude); Serial.print(F(","));
        Serial.print(float(cansat_data.temperature) / 100); Serial.print(F(","));
        Serial.print(long(cansat_data.pressure) * 10); Serial.print(F(","));
        Serial.print(millis()); Serial.print(F(","));
        Serial.println(millis() - previous_time);
        previous_time = millis();
    }
  }
}
