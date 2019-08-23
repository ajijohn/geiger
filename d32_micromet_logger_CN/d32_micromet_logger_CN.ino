const char* nodeID = "AG-05-MM-CN-01";

// Deep sleep
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600     /* Time ESP32 will go to sleep (in seconds). 600 seconds = 10 minutes */
#include "driver/adc.h"

#define TIME_TO_STAY_ON = 120000 /* How long to stay awake to take measurements, in milliseconds.*/

// E-Paper Display
#include <LOLIN_EPD.h>
#include <Adafruit_GFX.h>

#define EPD_CS 14
#define EPD_DC 27
#define EPD_RST 33  // can set to -1 and share with microcontroller Reset! Must be set to pin 33 to work with lolin cable
#define EPD_BUSY -1 // can set to -1 to not use a pin (will wait a fixed delay)
LOLIN_IL3897 EPD(250, 122, EPD_DC, EPD_RST, EPD_CS, EPD_BUSY); //hardware SPI

#include "Type4051Mux.h"
Type4051Mux mux(34, INPUT, ANALOG, 0, 2, 5);

#include <Anemometer.h>
#include <Wire.h>

#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

RtcDateTime now;
char datestring[20];

#include <OneWire.h> // Download from Arduino IDE Manage Libraries...
#include <DallasTemperature.h> // Download from Arduino IDE Manage Libraries...

#include <SPI.h>
#include <SD.h>

// Chip select for SD card.
const int CHIPSELECT = 4;

// Data wire is plugged into GPIO26 on the Arduino
#define ONE_WIRE_BUS 26

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses of DS18B20's
DeviceAddress therm1, therm2, therm3, therm4, therm5, therm6;

Anemometer anemometer1(25); // for AV06, anemometer 1 is on GPIO 25
Anemometer anemometer2(13);
Anemometer anemometer3(39);
Anemometer anemometer4(15); // for AV06, anemometer 4 is on GPIO 15

float solarIrradiance;
unsigned long prevMillis = 0;

unsigned long onTime;

// need to use floats instead of ints for the D32 Pro for some reason.
float direction1;
float direction2;
float direction3;
float direction4;

char therm1Address[17] = {};
char therm2Address[17] = {};
char therm3Address[17] = {};
char therm4Address[17] = {};
char therm5Address[17] = {};
char therm6Address[17] = {};

float temp1;
float temp2;
float temp3;
float temp4;
float temp5;
float temp6;

boolean dataSaved = false;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      EPD.clearBuffer();
      EPD.print("Woke up from deep sleep");
      EPD.display();
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

void setup() {
  Serial.begin(115200);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  print_wakeup_reason();

  EPD.begin();
  EPD.clearBuffer();
  EPD.fillScreen(EPD_WHITE);

  EPD.setCursor(0, 0);
  EPD.setTextColor(EPD_BLACK);
  EPD.setTextWrap(true);
  EPD.print("Micromet CN Node ");
  EPD.println(nodeID);
  EPD.display();

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) 
  {
    if (Rtc.LastError() != 0)
    {
        // we have a communications error
        // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
        // what the number means
        Serial.print("RTC communications error = ");
        Serial.println(Rtc.LastError());
    }
    else
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");

        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue

        Rtc.SetDateTime(compiled);
    }
  }

  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }

  now = Rtc.GetDateTime();
  if (now < compiled) 
  {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

  Serial.print(F("Initializing SD card..."));

  // see if the card is present and can be initialized:
  if (!SD.begin(CHIPSELECT)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    while (1);
  }
  Serial.println(F("card initialized."));
  
  Wire.begin();

  sensors.begin();
  delay(100);
  // locate devices on the bus
  Serial.print(F("Locating DS18B20 devices..."));
  Serial.print(F("Found "));
  delay(100);
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

  sprintf(therm1Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm1[0],therm1[1],therm1[2],therm1[3],therm1[4],therm1[5],therm1[6],therm1[7]);
  sprintf(therm2Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm2[0],therm2[1],therm2[2],therm2[3],therm2[4],therm2[5],therm2[6],therm2[7]);
  sprintf(therm3Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm3[0],therm3[1],therm3[2],therm3[3],therm3[4],therm3[5],therm3[6],therm3[7]);
  sprintf(therm4Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm4[0],therm4[1],therm4[2],therm4[3],therm4[4],therm4[5],therm4[6],therm4[7]);
  sprintf(therm5Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm5[0],therm5[1],therm5[2],therm5[3],therm5[4],therm5[5],therm5[6],therm5[7]);
  sprintf(therm6Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm6[0],therm6[1],therm6[2],therm6[3],therm6[4],therm6[5],therm6[6],therm6[7]);
  
  Serial.println(F("Device addresses: "));
  Serial.println(therm1Address);
  Serial.println(therm2Address);
  Serial.println(therm3Address);
  Serial.println(therm4Address);
  Serial.println(therm5Address);
  Serial.println(therm6Address);

  snprintf_P(datestring, 
          countof(datestring),
          PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
          now.Month(),
          now.Day(),
          now.Year(),
          now.Hour(),
          now.Minute(),
          now.Second() );
  
  anemometer1.setup();
  anemometer1.setInterval(110000);
  anemometer1.setDebugMode(true);
  anemometer2.setup();
  anemometer2.setInterval(110000);
  anemometer2.setDebugMode(true);
  anemometer3.setup();
  anemometer3.setInterval(110000);
  anemometer3.setDebugMode(true);
  anemometer4.setup();
  anemometer4.setInterval(110000);
  anemometer4.setDebugMode(true);

  Serial.print(F("Micro-met CN Node: "));
  Serial.println(nodeID);
}

void loop() {
  onTime = millis();
  
  anemometer1.update();
  anemometer2.update();
  anemometer3.update();
  anemometer4.update();
  
  if (millis() - prevMillis > 5000) {
    if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
            // what the number means
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }

    now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
    
    snprintf_P(datestring, 
          countof(datestring),
          PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
          now.Month(),
          now.Day(),
          now.Year(),
          now.Hour(),
          now.Minute(),
          now.Second() );
  
    Serial.print(F("Requesting temperatures..."));
    sensors.requestTemperatures();
    temp1 = sensors.getTempC(therm1);
    temp2 = sensors.getTempC(therm2);
    temp3 = sensors.getTempC(therm3);
    temp4 = sensors.getTempC(therm4);
    temp5 = sensors.getTempC(therm5);
    temp6 = sensors.getTempC(therm6);

    Serial.println(F("DONE"));
    Serial.print(F("Temperature for the device 1 (index 0) is: "));
    Serial.println(temp1);
    Serial.print(F("Temperature for the device 2 (index 1) is: "));
    Serial.println(temp2);
    Serial.print(F("Temperature for the device 3 (index 2) is: "));
    Serial.println(temp3);
    Serial.print(F("Temperature for the device 4 (index 3) is: "));
    Serial.println(temp4);
    Serial.print("Temperature for the device 5 (index 4) is: ");
    Serial.println(temp5);
    Serial.print("Temperature for the device 6 (index 5) is: ");
    Serial.println(temp6);

    // channel 4 = solar
    // channels 1, 2, 3, and 8 = wind direction

    Serial.println(F("Reading values from weather vanes..."));

    direction1 = mux.read(0); // for AV-06, first one is on channel 3
    delay(20);
    direction2 = mux.read(1); // channel 0 is second
    delay(20);
    direction3 = mux.read(2); // channel 2 is third
    delay(20);
    direction4 = mux.read(3); // for AV-06, last one is on channel 0

    Serial.print(F("VaneValue1: "));
    Serial.println(direction1);
    Serial.print(F("VaneValue2: "));
    Serial.println(direction2);
    Serial.print(F("VaneValue3: "));
    Serial.println(direction3);
    Serial.print(F("VaneValue4: "));
    Serial.println(direction4);

//    int data;
//    for (byte i = 0; i < 16; ++i) {
//      // Reads from channel i (returns a value from 0 to 1023).
//      data = mux.read(i);
//  
//      Serial.print("Potentiometer at channel ");
//      Serial.print(i);
//      Serial.print(" is at ");
//      Serial.print((double) (data) * 100 / 1023);
//      Serial.println("%%");
//    }
//    Serial.println();

    solarIrradiance = mux.read(4);
    Serial.println(F("Solar irradiance: "));
    Serial.println(solarIrradiance);
    
//    for (byte i = 0; i < 9; ++i) {
//      // Reads from channel i (returns a value from 0 to 4095).
//      direction1 = mux.read(i);
//  
//      Serial.print("Wind vane at channel ");
//      Serial.print(i);
//      Serial.print(" is at ");
//      Serial.println(direction1);
//    }

    File dataFile = SD.open("/datalog.csv", FILE_APPEND);
    if (dataFile) {
      dataFile.print(datestring);
      dataFile.print(",");
      dataFile.print(nodeID);
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
      dataFile.print(therm1Address);
      dataFile.print(",");
      dataFile.print(therm2Address);
      dataFile.print(",");
      dataFile.print(therm3Address);
      dataFile.print(",");
      dataFile.print(therm4Address);
      dataFile.print(",");
      dataFile.print(therm5Address);
      dataFile.print(",");
      dataFile.print(therm6Address);
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
      dataFile.println(solarIrradiance);
      dataFile.close();
      Serial.println(F("Data written to SD Card"));
      dataSaved = true;
    } else {
      dataSaved = false;
      EPD.clearBuffer();
      EPD.setCursor(0, 0);
      EPD.print("Warning! Unable to save data to SD card for Node ");
      EPD.print(nodeID);
      EPD.print(" at time: ");
      EPD.print(datestring);
      EPD.println(". ");
      EPD.println("Check to make sure SD card is properly seated in slot.");
      EPD.display();
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

    EPD.clearBuffer();
    EPD.setCursor(0, 0);
    if (dataSaved) {
      EPD.print("Last data recorded for Node ");
    } else {
      EPD.print("Warning! Data not recorded. Check if SD card is seated properly in slot. Last measured values for Node ");
    }
    EPD.print(nodeID);
    EPD.println(":");
    EPD.print("Time: ");
    EPD.println(datestring);
    EPD.print("Temp 1: ");
    EPD.print(temp1);
    EPD.print("; ");
    EPD.print("Temp 2: ");
    EPD.print(temp2);
    EPD.print("; ");
    EPD.print("Temp 3: ");
    EPD.print(temp3);
    EPD.print("; ");
    EPD.print("Temp 4: ");
    EPD.print(temp4);
    EPD.print("; ");
    EPD.print("Temp 5: ");
    EPD.print(temp5);
    EPD.print("; ");
    EPD.print("Temp 6: ");
    EPD.print(temp6);
    EPD.println("; ");
    EPD.print("Wind Directions: ");
    EPD.print(direction1);
    EPD.print("; ");
    EPD.print(direction2);
    EPD.print("; ");
    EPD.print(direction3);
    EPD.print("; ");
    EPD.print(direction4);
    EPD.print("; ");
    EPD.print("Wind Speeds: ");
    EPD.print(anemometer1.getWindSpeedKMPH());
    EPD.print("; ");
    EPD.print(anemometer2.getWindSpeedKMPH());
    EPD.print("; ");
    EPD.print(anemometer3.getWindSpeedKMPH());
    EPD.print("; ");
    EPD.print(anemometer4.getWindSpeedKMPH());
    EPD.println("; ");
    EPD.print("Solar Irradiance: ");
    EPD.print(solarIrradiance);
    EPD.display();

    if (onTime > 120000) {
      Serial.println(F("Entering deep sleep"));
      EPD.clearBuffer();
      EPD.setCursor(0, 0);
      EPD.print("Node ");
      EPD.print(nodeID);
      EPD.println(" status: sleeping");
      EPD.println("Last data prior to entering deep sleep:");
      EPD.print("Time: ");
      EPD.println(datestring);
      EPD.print("Temp 1: ");
      EPD.print(temp1);
      EPD.print("; ");
      EPD.print("Temp 2: ");
      EPD.print(temp2);
      EPD.print("; ");
      EPD.print("Temp 3: ");
      EPD.print(temp3);
      EPD.print("; ");
      EPD.print("Temp 4: ");
      EPD.print(temp4);
      EPD.print("; ");
      EPD.print("Temp 5: ");
      EPD.print(temp5);
      EPD.print("; ");
      EPD.print("Temp 6: ");
      EPD.print(temp6);
      EPD.println("; ");
      EPD.print("Wind Directions: ");
      EPD.print(direction1);
      EPD.print("; ");
      EPD.print(direction2);
      EPD.print("; ");
      EPD.print(direction3);
      EPD.print("; ");
      EPD.print(direction4);
      EPD.print("; ");
      EPD.print("Wind Speeds: ");
      EPD.print(anemometer1.getWindSpeedKMPH());
      EPD.print("; ");
      EPD.print(anemometer2.getWindSpeedKMPH());
      EPD.print("; ");
      EPD.print(anemometer3.getWindSpeedKMPH());
      EPD.print("; ");
      EPD.print(anemometer4.getWindSpeedKMPH());
      EPD.println("; ");
      EPD.print("Solar Irradiance: ");
      EPD.print(solarIrradiance);
      EPD.display();
      adc_power_off();
      esp_deep_sleep_start();
    }
    
    prevMillis = millis();
  }
}
