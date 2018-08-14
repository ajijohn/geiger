#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include "SeeeduinoStalker.h"
#include <Wire.h>
#include "DS1337.h"
#include <Anemometer.h>

// Data wire is plugged into port 7 on the Arduino
#define ONE_WIRE_BUS 7

// Anemometers
Anemometer anemometer1(2);
Anemometer anemometer2(3);
Anemometer anemometer3(4);
Anemometer anemometer4(5);

unsigned long previousMillis = 0;
/********************/

float NODEID = 1.0;

// Chip select for SD card.
const int CHIPSELECT = 10;

// Stalker data structure for reading battery voltage
Stalker stalker;

// Realtime clock setup
DS1337 RTC;

//year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
//writing any non-existent time-data may interfere with normal operation of the RTC.
//Take care of week-day also.
DateTime dt(2018, 5, 25, 18, 57, 0, 0);

//Use default 9-bit precision
//#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress therm1, therm2, therm3, therm4, therm5, therm6;

// Assign address manually. The addresses below will beed to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
// DeviceAddress insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// DeviceAddress outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  delay(10);
  Serial.println(F("DotMote Temperature Datalogger"));
  Serial.println(NODEID);

  // LED on shield
  pinMode(4, OUTPUT);

  Wire.begin();
  RTC.begin();
  RTC.adjust(dt); //Adjust date-time as defined 'dt' above 

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // enable 3V3 pin
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  delay(100);

  anemometer1.setup();
  anemometer2.setup();
  anemometer3.setup();
  anemometer4.setup();

  Serial.print(F("Initializing SD card..."));

  // see if the card is present and can be initialized:
  if (!SD.begin(CHIPSELECT)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    while (1);
  }
  Serial.println(F("card initialized."));
  
  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print(F("Locating devices..."));
  Serial.print(F("Found "));
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(F(" devices."));

  // report parasite power requirements
  Serial.print(F("Parasite power is: "));
  if (sensors.isParasitePowerMode()) {
    Serial.println(F("ON"));
  } else {
    Serial.println(F("OFF"));
  };

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

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them. It might be a good idea to
  // check the CRC to make sure you didn't get garbage. The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  // show the addresses we found on the bus
  Serial.print(F("therm1 Address: "));
  printAddress(therm1);
  Serial.println();

  Serial.print(F("therm2 Address: "));
  printAddress(therm2);
  Serial.println();

  Serial.print(F("therm3 Address: "));
  printAddress(therm3);
  Serial.println();

  Serial.print(F("therm4 Address: "));
  printAddress(therm4);
  Serial.println();

  Serial.print(F("therm5 Address: "));
  printAddress(therm5);
  Serial.println();

  Serial.print(F("therm6 Address: "));
  printAddress(therm6);
  Serial.println();

//  set the resolution to 9 bit per device
//  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
//  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  Serial.print(F("therm1 Resolution: "));
  Serial.print(sensors.getResolution(therm1), DEC);
  Serial.println();

  Serial.print(F("therm2 Resolution: "));
  Serial.print(sensors.getResolution(therm2), DEC);
  Serial.println();

  Serial.print(F("therm3 Resolution: "));
  Serial.print(sensors.getResolution(therm3), DEC);
  Serial.println();

  Serial.print(F("therm4 Resolution: "));
  Serial.print(sensors.getResolution(therm4), DEC);
  Serial.println();

  Serial.print(F("therm5 Resolution: "));
  Serial.print(sensors.getResolution(therm5), DEC);
  Serial.println();

  Serial.print(F("therm6 Resolution: "));
  Serial.print(sensors.getResolution(therm6), DEC);
  Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print(F("0"));
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(F("Temp C: "));
  Serial.print(tempC);
  Serial.print(F(" Temp F: "));
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print(F("Resolution: "));
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

void printWindSpeed(Anemometer anem) {
  Serial.print(F("Wind Speed: "));
  Serial.print(anem.getWindSpeedMPH());
  Serial.println(F(" mph"));
}

// main function to print information about a thermistor
void printThermistorData(DeviceAddress deviceAddress)
{
  Serial.print(F("Device Address: "));
  printAddress(deviceAddress);
  Serial.print(F(" "));
  printTemperature(deviceAddress);
  Serial.println();
}

void printBatteryVoltage() {
  float voltage = stalker.readBattery();
  Serial.print(F("Battery voltage: "));
  Serial.println(voltage, 2);
}

float getBatteryVoltage() {
  return stalker.readBattery();
}

void writeDataToCard(DeviceAddress deviceAddress) {
  DateTime now = RTC.now(); //get the current date-time
  
  Serial.print(now.year(), DEC);
  Serial.print(F("/"));
  Serial.print(now.month(), DEC);
  Serial.print(F("/"));
  Serial.print(now.date(), DEC);
  Serial.print(F(" "));
  Serial.print(now.hour(), DEC);
  Serial.print(F(":"));
  Serial.print(now.minute(), DEC);
  Serial.print(F(":"));
  Serial.print(now.second(), DEC);
  Serial.println();
  
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  
  if (dataFile) {
    dataFile.print(now.year());
    dataFile.print("/");
    dataFile.print(now.month());
    dataFile.print("/");
    dataFile.print(now.date());
    dataFile.print(" ");
    dataFile.print(now.hour());
    dataFile.print(":");
    dataFile.print(now.minute());
    dataFile.print(":");
    dataFile.print(now.second());
    dataFile.print(",");
    for (uint8_t i = 0; i < 8; i++) {
      // zero pad the address if necessary
      if (deviceAddress[i] < 16) dataFile.print(F("0"));
      dataFile.print(deviceAddress[i], HEX);
    }
    dataFile.print(",");
    dataFile.print(sensors.getTempC(deviceAddress));
    dataFile.print(",");
    dataFile.println(getBatteryVoltage());
    dataFile.close();
    Serial.println(F("Data written to SD Card"));
  } else {
    Serial.println(F("Error opening datalog.csv"));
  }
}

/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
  digitalWrite(6, HIGH);
  printBatteryVoltage();
  anemometer1.update();
  anemometer2.update();
  anemometer3.update();
  anemometer4.update();

  Serial.print("Anemometer 1: ");
  printWindSpeed(anemometer1);
  Serial.print("Anemometer 2: ");
  printWindSpeed(anemometer2);
  Serial.print("Anemometer 3: ");
  printWindSpeed(anemometer3);
  Serial.print("Anemometer 4: ");
  printWindSpeed(anemometer4);
  
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print(F("Requesting temperatures..."));
  sensors.requestTemperatures();
  Serial.println(F("DONE"));

  // print the device information
  printThermistorData(therm1);
  printThermistorData(therm2);
  printThermistorData(therm3);
  printThermistorData(therm4);
  printThermistorData(therm5);
  printThermistorData(therm6);

  writeDataToCard(therm1);
  writeDataToCard(therm2);
  writeDataToCard(therm3);
  writeDataToCard(therm4);
  writeDataToCard(therm5);
  writeDataToCard(therm6);

  digitalWrite(6, LOW);

  delay(10000);
}