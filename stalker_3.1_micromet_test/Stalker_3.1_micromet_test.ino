#include <Anemometer.h>
#include "DS1337.h"
#include <Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 7 on the Arduino
#define ONE_WIRE_BUS 7

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Commented out the RTC code. Can't use on-board RTC because it uses up pins A4 and A5 for I2C communication
//DS1337 RTC;
//DateTime dt(2018, 7, 16, 19, 30, 0, 1);

Anemometer anemometer1(8);
Anemometer anemometer2(3);
Anemometer anemometer3(6);
Anemometer anemometer4(A4);
unsigned long prevMillis = 0;

void setup() {
  Serial.begin(115200);
  // enable 3V3 pin
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  delay(100);

  Wire.begin();

//  RTC.begin();
//  RTC.adjust(dt); //Adjust date-time as defined 'dt' above 
  sensors.begin();
  
  anemometer1.setup();
  anemometer1.setDebugMode(true);
  anemometer2.setup();
  anemometer2.setDebugMode(true);
  anemometer3.setup();
  anemometer3.setDebugMode(true);
  anemometer4.setup();
  anemometer4.setDebugMode(true);

  Serial.println("Micro-met Test");
}

void loop() {
//  DateTime now; //get the current date-time
  anemometer1.update();
  anemometer2.update();
  anemometer3.update();
  anemometer4.update();
  
  if (millis() - prevMillis > 5000) {
//    now = RTC.now();
//    Serial.print(F("The time is now: "));
//    Serial.print(now.year(), DEC);
//    Serial.print(F("/"));
//    Serial.print(now.month(), DEC);
//    Serial.print(F("/"));
//    Serial.print(now.date(), DEC);
//    Serial.print(F(" "));
//    Serial.print(now.hour(), DEC);
//    Serial.print(F(":"));
//    Serial.print(now.minute(), DEC);
//    Serial.print(F(":"));
//    Serial.print(now.second(), DEC);
//    Serial.println();

    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(sensors.getTempCByIndex(0));
    Serial.print("Temperature for the device 2 (index 1) is: ");
    Serial.println(sensors.getTempCByIndex(1));
    Serial.print("Temperature for the device 3 (index 2) is: ");
    Serial.println(sensors.getTempCByIndex(2));
    Serial.print("Temperature for the device 4 (index 3) is: ");
    Serial.println(sensors.getTempCByIndex(3));
    Serial.print("Temperature for the device 5 (index 4) is: ");
    Serial.println(sensors.getTempCByIndex(4));
    Serial.print("Temperature for the device 6 (index 5) is: ");
    Serial.println(sensors.getTempCByIndex(5));
  
    Serial.print(F("The last recorded speed for anemometer1 was: "));
    Serial.print(anemometer1.getWindSpeedKMPH());
    Serial.println(F(" km/hour"));
    Serial.print(F("The last recorded speed for anemometer2 was: "));
    Serial.print(anemometer2.getWindSpeedKMPH());
    Serial.println(F(" km/hour"));
    Serial.print(F("The last recorded speed for anemometer3 was: "));
    Serial.println(anemometer3.getWindSpeedKMPH());
    Serial.println(F(" km/hour"));
    Serial.print(F("The last recorded speed for anemometer4 was: "));
    Serial.println(anemometer4.getWindSpeedKMPH());
    Serial.println(F(" km/hour"));

    Serial.println(F("Reading values from weather vanes..."));
    Serial.print(F("VaneValue1: "));
    Serial.println(analogRead(A0));
    delay(50);
    Serial.print(F("VaneValue2: "));
    Serial.println(analogRead(A1));
    delay(50);
    Serial.print(F("VaneValue3: "));
    Serial.println(analogRead(A2));
    delay(50);
    Serial.print(F("VaneValue4: "));
    Serial.println(analogRead(A3));
    delay(50);
    
    Serial.println(F("Solar irradiance: "));
    Serial.println(analogRead(A5));
    delay(50);
    prevMillis = millis();
  }
}
