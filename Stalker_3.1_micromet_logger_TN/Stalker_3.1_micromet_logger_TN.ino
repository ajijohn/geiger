#include <OneWire.h> // Download from Arduino IDE Manage Libraries...
#include <DallasTemperature.h> // Download from Arduino IDE Manage Libraries...
#include <SPI.h>
#include <SD.h>
#include "SeeeduinoStalker.h" // Get library from: https://github.com/Seeed-Studio/Sketch_Stalker_V3_1/tree/master/libraries/Stalker_V3_1
#include <Wire.h>
#include "DS1337.h" // Get library from: https://github.com/Seeed-Studio/Sketch_Stalker_V3_1/tree/master/libraries/DS1337

// Data wire is plugged into port 7 on the Arduino
#define ONE_WIRE_BUS 7

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
//DateTime dt(2018, 7, 9, 19, 5, 0, 1);

//Use default 9-bit precision
//#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress therm1, therm2, therm3, therm4;

// Assign address manually. The addresses below will beed to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
// DeviceAddress insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// DeviceAddress outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

char therm1Address[17] = {};
char therm2Address[17] = {};
char therm3Address[17] = {};
char therm4Address[17] = {};
//char therm5Address[17] = {};
//char therm6Address[17] = {};
//char therm7Address[17] = {};

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
//  RTC.adjust(dt); //Adjust date-time as defined 'dt' above 

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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
//  if (!sensors.getAddress(therm5, 4)) Serial.println(F("Unable to find address for therm5"));
//  if (!sensors.getAddress(therm6, 5)) Serial.println(F("Unable to find address for therm6"));
//  if (!sensors.getAddress(therm7, 6)) Serial.println(F("Unable to find address for therm7"));

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

  sprintf(therm1Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm1[0],therm1[1],therm1[2],therm1[3],therm1[4],therm1[5],therm1[6],therm1[7]);
  sprintf(therm2Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm2[0],therm2[1],therm2[2],therm2[3],therm2[4],therm2[5],therm2[6],therm2[7]);
  sprintf(therm3Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm3[0],therm3[1],therm3[2],therm3[3],therm3[4],therm3[5],therm3[6],therm3[7]);
  sprintf(therm4Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm4[0],therm4[1],therm4[2],therm4[3],therm4[4],therm4[5],therm4[6],therm4[7]);
//  sprintf(therm5Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm5[0],therm5[1],therm5[2],therm5[3],therm5[4],therm5[5],therm5[6],therm5[7]);
//  sprintf(therm6Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm6[0],therm6[1],therm6[2],therm6[3],therm6[4],therm6[5],therm6[6],therm6[7]);
//  sprintf(therm7Address, "%02X%02X%02X%02X%02X%02X%02X%02X", therm7[0],therm7[1],therm7[2],therm7[3],therm7[4],therm7[5],therm7[6],therm7[7]);

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

//  Serial.print(F("therm5 Address: "));
//  printAddress(therm5);
//  Serial.println();
//
//  Serial.print(F("therm6 Address: "));
//  printAddress(therm6);
//  Serial.println();
//
//  Serial.print(F("therm7 Address: "));
//  printAddress(therm7);
//  Serial.println();

  DateTime now = RTC.now(); //get the current date-time

  File addressDataFile = SD.open("address.csv", FILE_WRITE);
  if (addressDataFile) {
    addressDataFile.print(now.year());
    addressDataFile.print("/");
    addressDataFile.print(now.month());
    addressDataFile.print("/");
    addressDataFile.print(now.date());
    addressDataFile.print(" ");
    addressDataFile.print(now.hour());
    addressDataFile.print(":");
    addressDataFile.print(now.minute());
    addressDataFile.print(":");
    addressDataFile.println(now.second());
    addressDataFile.print("therm1");
    addressDataFile.print(",");
    addressDataFile.println(therm1Address);
    addressDataFile.print("therm2");
    addressDataFile.print(",");
    addressDataFile.println(therm2Address);
    addressDataFile.print("therm3");
    addressDataFile.print(",");
    addressDataFile.println(therm3Address);
    addressDataFile.print("therm4");
    addressDataFile.print(",");
    addressDataFile.println(therm4Address);
//    addressDataFile.print("therm5");
//    addressDataFile.print(",");
//    addressDataFile.println(therm5Address);
//    addressDataFile.print("therm6");
//    addressDataFile.print(",");
//    addressDataFile.println(therm6Address);
//    addressDataFile.print("therm7");
//    addressDataFile.print(",");
//    addressDataFile.println(therm7Address);
    addressDataFile.close();
    Serial.println(F("Thermistor addresses recorded."));
  }

//  set the resolution to 9 bit per device
//  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
//  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

//  Serial.print(F("therm1 Resolution: "));
//  Serial.print(sensors.getResolution(therm1), DEC);
//  Serial.println();
//
//  Serial.print(F("therm2 Resolution: "));
//  Serial.print(sensors.getResolution(therm2), DEC);
//  Serial.println();
//
//  Serial.print(F("therm3 Resolution: "));
//  Serial.print(sensors.getResolution(therm3), DEC);
//  Serial.println();
//
//  Serial.print(F("therm4 Resolution: "));
//  Serial.print(sensors.getResolution(therm4), DEC);
//  Serial.println();
//
//  Serial.print(F("therm5 Resolution: "));
//  Serial.print(sensors.getResolution(therm5), DEC);
//  Serial.println();
//
//  Serial.print(F("therm6 Resolution: "));
//  Serial.print(sensors.getResolution(therm6), DEC);
//  Serial.println();
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

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
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
  if (now.minute() < 10) {
    Serial.print("0");
  } 
  Serial.print(now.minute(), DEC);
  Serial.print(F(":"));
  if (now.second() < 10) {
    Serial.print("0");
  } 
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
    if (now.minute() < 10) {
      dataFile.print("0");
    } 
    dataFile.print(now.minute());
    dataFile.print(":");
    if (now.second() < 10) {
      dataFile.print("0");
    } 
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
  digitalWrite(4, HIGH);
  printBatteryVoltage();
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print(F("Requesting temperatures..."));
  sensors.requestTemperatures();
  Serial.println(F("DONE"));

  // print the device information
  printData(therm1);
  printData(therm2);
  printData(therm3);
  printData(therm4);
//  printData(therm5);
//  printData(therm6);
//  printData(therm7);

  writeDataToCard(therm1);
  writeDataToCard(therm2);
  writeDataToCard(therm3);
  writeDataToCard(therm4);
//  writeDataToCard(therm5);
//  writeDataToCard(therm6);
//  writeDataToCard(therm7);

  digitalWrite(4, LOW);

  delay(180000);
}