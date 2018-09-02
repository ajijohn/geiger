/*
  SD card basic file example

 This example shows how to create and destroy an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <OneWire.h> // install from Arduino IDE "Manage libraries..."
#include <DallasTemperature.h> // install from Arduino IDE "Manage libraries..."
const int SDChipSelect = 10; // SS pin on the SD card.

// Data wire is plugged into port 7 on the Arduino
#define ONE_WIRE_BUS 7

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses of DS18B20's
DeviceAddress therm1, therm2, therm3, therm4;

char therm1Address[17];
char therm2Address[17];
char therm3Address[17];
char therm4Address[17];

File myFile;
File root;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // enable 3V3 pin
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  delay(100);

  Serial.print("Initializing SD card...");

  if (!SD.begin(SDChipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

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

  Serial.print(F("Address for therm1: "));
  printAddress(therm1);
  Serial.print(F("Address for therm2: "));
  printAddress(therm2);
  Serial.print(F("Address for therm3: "));
  printAddress(therm3);
  Serial.print(F("Address for therm4: "));
  printAddress(therm4);

  root = SD.open("/");
  printDirectoryAndRemove(root, 0);
  
  if (SD.exists("datalog.txt")) {
    Serial.println(F("datalog.txt exists."));
  } else {
    Serial.println(F("datalog.txt doesn't exist."));
  }

  // Check to see if the file exists:
  if (SD.exists("DATALOG.CSV")) {
    Serial.println(F("DATALOG.CSV exists."));
  } else {
    Serial.println(F("DATALOG.CSV doesn't exist."));
  }

  // delete the file:
  //Serial.println("Removing example.txt...");
  //SD.remove("example.txt");

  //if (SD.exists("example.txt")) {
  //  Serial.println("example.txt exists.");
  //} else {
  //  Serial.println("example.txt doesn't exist.");
  //}

  
}

void loop() {
  // nothing happens after setup finishes.
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  float temp1 = sensors.getTempC(therm1);
  float temp2 = sensors.getTempC(therm2);
  float temp3 = sensors.getTempC(therm3);
  float temp4 = sensors.getTempC(therm4);

  Serial.println("DONE");
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.print(temp1);
  Serial.print(F(" Device address: "));
  printAddress(therm1);
  
  Serial.print("Temperature for the device 2 (index 1) is: ");
  Serial.print(temp2);
  Serial.print(F(" Device address: "));
  printAddress(therm2);
  
  Serial.print("Temperature for the device 3 (index 2) is: ");
  Serial.print(temp3);
  Serial.print(F(" Device address: "));
  printAddress(therm3);
  
  Serial.print("Temperature for the device 4 (index 3) is: ");
  Serial.print(temp4);
  Serial.print(F(" Device address: "));
  printAddress(therm4);

  delay(10000);
}

void printDirectory(File dir, int numTabs)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (! entry)
    {
      if (numTabs == 0)
        Serial.println("** Done **");
      return;
    }
    for (uint8_t i = 0; i < numTabs; i++)
      Serial.print('\t');
      Serial.print(entry.name());
      
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void printDirectoryAndRemove(File dir, int numTabs)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (! entry)
    {
      if (numTabs == 0)
        Serial.println("** Done **");
      return;
    }
    for (uint8_t i = 0; i < numTabs; i++)
      Serial.print('\t');
      Serial.print(entry.name());

    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
      Serial.println("Removing " + (String)entry.name());
      SD.remove(entry.name());
      Serial.println("Removed " + (String)entry.name());
      


    }
    entry.close();
  }
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
  Serial.println(F(""));
}

