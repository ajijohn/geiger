#include <Anemometer.h>

Anemometer anemometer1(5);
unsigned long prevMillis = 0;

void setup() {
  Serial.begin(115200);
  anemometer1.setup();
}

void loop() {
  anemometer1.update();
  if (millis() - prevMillis > 5000) {
    Serial.print(F("The last recorded speed for anemometer1 was: "));
    Serial.println(anemometer1.getWindSpeedKMPH());
    prevMillis = millis();
  }
}