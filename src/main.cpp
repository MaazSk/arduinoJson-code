#include <Arduino.h>
#include <ArduinoJson.h>

#include "gsmdeclarations.hpp"
#include "Serial_data.h"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  // GSM_INIT();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSerial1();
}