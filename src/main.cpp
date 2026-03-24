#include <Arduino.h>
#include "Hourglass.h"

Hourglass hourglass;
void setup() {
  hourglass.init();
}

void loop() {
  hourglass.update(millis());
}