#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "DS1337.h"
#include <OneWire.h>

OneWire  ds(7);  // on pin 7

DS1337 RTC; //Create the DS1337 object
char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const int chipSelect = 10;
DateTime dt(2016, 7, 28, 12, 22, 0, 4);

void setup(void) {
  Serial.begin(9600);
  
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  Wire.begin();
  RTC.begin();
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  DateTime now = RTC.now(); //get the current date-time
  dataFile.print("Time = ");
  dataFile.print(now.year(), DEC);
  dataFile.print('/');
  dataFile.print(now.month(), DEC);
  dataFile.print('/');
  dataFile.print(now.date(), DEC);
  dataFile.print(' ');
  dataFile.print(now.hour(), DEC);
  dataFile.print(':');
  dataFile.print(now.minute(), DEC);
  dataFile.print(':');
  dataFile.print(now.second(), DEC);
  
//  Serial.print(now.year(), DEC);
//  Serial.print('/');
//  Serial.print(now.month(), DEC);
//  Serial.print('/');
//  Serial.print(now.date(), DEC);
//  Serial.print(' ');
//  Serial.print(now.hour(), DEC);
//  Serial.print(':');
//  Serial.print(now.minute(), DEC);
//  Serial.print(':');
//  Serial.print(now.second(), DEC);
//  Serial.println();
//  Serial.print(weekDay[now.dayOfWeek()]);
//  Serial.println();

  dataFile.print("  ROM =");
  //Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    dataFile.write(' ');
    dataFile.print(addr[i], HEX);
    //Serial.write(' ');
    //Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      dataFile.println("CRC is not valid!");
      //Serial.println("CRC is not valid!");
      return;
  }
  //Serial.println();
 
  //the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
//      dataFile.println("  Chip = DS18S20");
//      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
//      dataFile.println("  Chip = DS18B20");
//      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
//      dataFile.println("  Chip = DS1822");
//      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
//      dataFile.println("Device is not a DS18x20 family device.");
//      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

//  dataFile.print("  Data = ");
//  dataFile.print(present,HEX);
//  dataFile.print(" ");
//  Serial.print("  Data = ");
//  Serial.print(present,HEX);
//  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
//    dataFile.print(data[i], HEX);
//    dataFile.print(" ");
//    Serial.print(data[i], HEX);
//    Serial.print(" ");
  }
//  dataFile.print(" CRC=");
//  dataFile.print(OneWire::crc8(data, 8), HEX);
//  dataFile.println();
//  Serial.print(" CRC=");
//  Serial.print(OneWire::crc8(data, 8), HEX);
//  Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
//  Serial.print("  Temperature = ");
//  Serial.print(celsius);
//  Serial.print(" Celsius, ");
//  Serial.print(fahrenheit);
//  Serial.println(" Fahrenheit");
  dataFile.print("  Temperature = ");
  dataFile.print(celsius);
  dataFile.print(" C  ");
  dataFile.print(fahrenheit);
  dataFile.println(" F");
  dataFile.close();

  delay(1000);

}
