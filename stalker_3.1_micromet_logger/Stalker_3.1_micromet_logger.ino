#include <Anemometer.h>
#include "DS1337.h"
#include <Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <SPI.h>
#include <SparkFunDS3234RTC.h> // <TODO: insert link for Sparkfun DS3234 RTC library>
#include <SD.h>
#include "SeeeduinoStalker.h"

// Stalker data structure for reading battery voltage
Stalker stalker;

// Chip select for SD card.
const int CHIPSELECT = 10;

// Comment out the line below if you want date printed before month.
// E.g. October 31, 2016: 10/31/16 vs. 31/10/16
#define PRINT_USA_DATE

//////////////////////////////////
// Configurable Pin Definitions //
//////////////////////////////////
#define DS13074_CS_PIN 5 // DeadOn RTC Chip-select pin

// Data wire is plugged into port 7 on the Arduino
#define ONE_WIRE_BUS 7

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses of DS18B20's
DeviceAddress therm1, therm2, therm3, therm4, therm5, therm6;

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

  Serial.print(F("Initializing SD card..."));

  // see if the card is present and can be initialized:
  if (!SD.begin(CHIPSELECT)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    while (1);
  }
  Serial.println(F("card initialized."));

  // Call rtc.begin([cs]) to initialize the library
  // The chip-select pin should be sent as the only parameter
  rtc.begin(DS13074_CS_PIN);
  //rtc.set12Hour(); // Use rtc.set12Hour to set to 12-hour mode

  // Now set the time...
  // You can use the autoTime() function to set the RTC's clock and
  // date to the compiler's predefined time. (It'll be a few seconds
  // behind, but close!)
  rtc.autoTime();
  // Or you can use the rtc.setTime(s, m, h, day, date, month, year)
  // function to explicitly set the time:
  // e.g. 7:32:16 | Monday October 31, 2016:
  //rtc.setTime(16, 32, 7, 2, 31, 10, 16);  // Uncomment to manually set time

  // Update time/date values, so we can set alarms
  rtc.update();

  Wire.begin();

  sensors.begin();
  // locate devices on the bus
  Serial.print(F("Locating DS18B20 devices..."));
  Serial.print(F("Found "));
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(F(" devices."));

  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index
  if (!sensors.getAddress(therm1, 0)) Serial.println(F("Unable to find address for therm1"));
  if (!sensors.getAddress(therm2, 1)) Serial.println(F("Unable to find address for therm2"));
  if (!sensors.getAddress(therm3, 2)) Serial.println(F("Unable to find address for therm3"));
  if (!sensors.getAddress(therm4, 3)) Serial.println(F("Unable to find address for therm4"));
  if (!sensors.getAddress(therm5, 4)) Serial.println(F("Unable to find address for therm5"));
  if (!sensors.getAddress(therm6, 5)) Serial.println(F("Unable to find address for therm6"));
  
  anemometer1.setup();
  anemometer1.setInterval(180000);
  anemometer1.setDebugMode(true);
  anemometer2.setup();
  anemometer2.setInterval(180000);
  anemometer2.setDebugMode(true);
  anemometer3.setup();
  anemometer3.setInterval(180000);
  anemometer3.setDebugMode(true);
  anemometer4.setup();
  anemometer4.setInterval(180000);
  anemometer4.setDebugMode(true);

  Serial.println("Micro-met Test");
}

void loop() {
  rtc.update();
  anemometer1.update();
  anemometer2.update();
  anemometer3.update();
  anemometer4.update();
  
  if (millis() - prevMillis > 5000) {
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures();
    float temp1 = sensors.getTempC(therm1);
    float temp2 = sensors.getTempC(therm2);
    float temp3 = sensors.getTempC(therm3);
    float temp4 = sensors.getTempC(therm4);
    float temp5 = sensors.getTempC(therm5);
    float temp6 = sensors.getTempC(therm6);

    //delay for 50 milliseconds to quiet Arduino ADC between readings
    int direction1 = analogRead(A0);
    delay(50);
    int direction2 = analogRead(A1);
    delay(50);
    int direction3 = analogRead(A2);
    delay(50);
    int direction4 = analogRead(A3);
    delay(50);
    int solarIrradiance = analogRead(A5);

    Serial.println("DONE");
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(temp1);
    Serial.print("Temperature for the device 2 (index 1) is: ");
    Serial.println(temp2);
    Serial.print("Temperature for the device 3 (index 2) is: ");
    Serial.println(temp3);
    Serial.print("Temperature for the device 4 (index 3) is: ");
    Serial.println(temp4);
    Serial.print("Temperature for the device 5 (index 4) is: ");
    Serial.println(temp5);
    Serial.print("Temperature for the device 6 (index 5) is: ");
    Serial.println(temp6);

    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.print(rtc.year());
      dataFile.print("/");
      dataFile.print(rtc.month());
      dataFile.print("/");
      dataFile.print(rtc.date());
      dataFile.print(" ");
      dataFile.print(rtc.hour());
      dataFile.print(":");
      dataFile.print(rtc.minute());
      dataFile.print(":");
      dataFile.print(rtc.second());
      dataFile.print(",");
      dataFile.print(temp1);
      dataFile.print(",");
      dataFile.print(temp2);
      dataFile.print(",");
      dataFile.print(temp3);
      dataFile.print(",");
      dataFile.print(temp4);
      dataFile.print(",");
      dataFile.print(temp5);
      dataFile.print(",");
      dataFile.print(temp6);
      dataFile.print(",");
      dataFile.print(anemometer1.getWindSpeedKMPH());
      dataFile.print(",");
      dataFile.print(anemometer2.getWindSpeedKMPH());
      dataFile.print(",");
      dataFile.print(anemometer3.getWindSpeedKMPH());
      dataFile.print(",");
      dataFile.print(anemometer4.getWindSpeedKMPH());
      dataFile.print(",");
      //write wind directions
      dataFile.print(direction1);
      dataFile.print(",");
      dataFile.print(direction2);
      dataFile.print(",");
      dataFile.print(direction3);
      dataFile.print(",");
      dataFile.print(direction4);
      dataFile.print(",");
      //write solar radiation level
      dataFile.print(solarIrradiance);
      dataFile.print(",");
      dataFile.println(getBatteryVoltage());
      dataFile.close();
      Serial.println(F("Data written to SD Card"));
    } else {
      Serial.println(F("Error opening datalog.csv"));
    }

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
    Serial.println(direction1);
    Serial.print(F("VaneValue2: "));
    Serial.println(direction2);
    Serial.print(F("VaneValue3: "));
    Serial.println(direction3);
    Serial.print(F("VaneValue4: "));
    Serial.println(direction4);
    
    Serial.println(F("Solar irradiance: "));
    Serial.println(solarIrradiance);
    prevMillis = millis();
  }
}

void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }
  
  Serial.print(" | ");

  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
#ifdef PRINT_USA_DATE
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
#else
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
                 String(rtc.month()) + "/"); // Print month
#endif
  Serial.println(String(rtc.year()));        // Print year
}

float getBatteryVoltage() {
  return stalker.readBattery();
}