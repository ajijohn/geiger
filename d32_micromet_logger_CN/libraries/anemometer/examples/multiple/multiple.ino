#include <Anemometer.h>

Anemometer anemometer1(5);
Anemometer anemometer2(6);
unsigned long prevMillis = 0;

void setup() {
  Serial.begin(115200);
  anemometer1.setup();
  anemometer2.setup();
}

void loop() {
  anemometer1.update();
  anemometer2.update();
  
  if (millis() - prevMillis > 5000) {
    Serial.print(F("The last recorded speed for anemometer1 was: "));
    Serial.println(anemometer1.getWindSpeedKMPH());
    Serial.print(F("The last recorded speed for anemometer2 was: "));
    Serial.println(anemometer2.getWindSpeedKMPH());
    prevMillis = millis();
  }
}