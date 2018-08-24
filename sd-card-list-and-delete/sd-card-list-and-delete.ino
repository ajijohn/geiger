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
const int SDChipSelect = 10; // SS pin on the SD card.

File myFile;
File root;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(SDChipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  root = SD.open("/");
  printDirectoryAndRemove(root, 0);
  
  if (SD.exists("datalog.txt")) {
    Serial.println("datalog.txt exists.");
  } else {
    Serial.println("datalog.txt doesn't exist.");
  }

  // Check to see if the file exists:
  if (SD.exists("DATALOG.CSV")) {
    Serial.println("DATALOG.CSV exists.");
  } else {
    Serial.println("DATALOG.CSV doesn't exist.");
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

