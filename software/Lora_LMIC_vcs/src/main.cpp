#include <Arduino.h>
#include "common.h"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  wifi_ota_setup();
}

void loop() {
  // put your main code here, to run repeatedly:

wifi_ota_loop();
}