#include <OneWire.h>
#include <SD.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#define NUM_DS18B20 1

const int offset=900;

// DS1307RTC
tmElements_t target; 

// SD Card
const int cs=9;
File logFile;
char fileName[]="log.csv";

// DS18B20
const int ow=7;
OneWire ds(ow);
byte owAddr[NUM_DS18B20][8], owData[NUM_DS18B20][12];

void setup(){
  // Setup Serial
  Serial.begin(9600);
  //while(!Serial) continue;
  delay(200);
  Serial.println("ABIO DataLogger");
  Serial.println("---------------");
  
  // Setup SD Card
  Serial.println("Initialising SD card...");
  pinMode(10, OUTPUT);
  if(!SD.begin(cs)){
    Serial.println("...failed!");
    return;
  }
  logFile=SD.open(fileName, FILE_WRITE);
  if(!logFile){
    Serial.print("...failed! Can't open ");
    Serial.print(fileName);
    Serial.println("!");
    return;
  }  
  Serial.println("...done!");
  
  // Setup RTC
  Serial.println("Initialising DS1307...");
  if(!readRTC(target)){
    Serial.println("...failed!");
    return;
  }

  Serial.println("...done!");
  
  // Setup DS18B20  
  for(int i=0; i<NUM_DS18B20; i++){
    Serial.println("Initialising DS18B20...");
    if(!ds.search(owAddr[i])){
      Serial.println("...failed!");
      return;
    } else {
      logFile.print("Temp. Sensor ");
      logFile.print(i);
      logFile.print(": ");
      for(int j=0; j<8; j++){
        Serial.print(owAddr[i][j], HEX);
        logFile.print(owAddr[i][j], HEX);
      }
      Serial.println(); 
      logFile.println();
    }   
    
    Serial.println("...done!");
  
    if(OneWire::crc8(owAddr[i],7)!=owAddr[i][7]){
      Serial.println("CRC Failed!");
      return;
    } 
  
    if(owAddr[i][0]!=0x28){
      Serial.println("OW Device is not DS18B20!"); 
    }
  }
 
  logFile.print("Date,Time,");
  for (int i=0; i<NUM_DS18B20; i++){
    logFile.print("Temperature ");
    logFile.print(i);
    if(i<NUM_DS18B20) logFile.print(',');
  }
  logFile.println();  
 
  Serial.println("Ready!");
}

void loop(){
  tmElements_t current;
  if(!readRTC(current)) return;  
  if(equal(current,target)){
    char buff[9];
    sprintf(buff, "%02d/%02d/%02d,", current.Day, current.Month, tmYearToY2k(current.Year));
    logFile.print(buff);
    sprintf(buff, "%02d:%02d:%02d,", current.Hour, current.Minute, current.Second);
    logFile.print(buff);  
    
    for(int i=0; i<NUM_DS18B20; i++){
      float celcius=readTemp(i);    
      dtostrf(celcius, 3, 4, buff);
      logFile.print(buff);
      if(i<NUM_DS18B20) logFile.print(',');
    }
    logFile.println();
    logFile.flush();
    increment(target, offset);
  }
  delay(1000);  
}

// DS1307 RTC Functions
int readRTC(tmElements_t &tm){
  if(RTC.read(tm)) return 1;
  else {
    if(RTC.chipPresent()){
      Serial.println("The DS1307 is stopped. Please run the SetTime");
      Serial.println("example to initialise the time and begin running.");
      return 0;
    } 
    else {
      Serial.println("DS1307 read error! Please check configuration!");
      return 0;
    } 
  }  
}

void increment(tmElements_t &tm, int o){
  breakTime(makeTime(tm)+o, tm);
}

int equal(tmElements_t &a, tmElements_t &b){
  return (makeTime(a)==makeTime(b)) ? 1 : 0;
}

// DS18B20 Functions
float readTemp(int j){
  ds.reset();
  ds.select(owAddr[j]);
  ds.write(0x44,1);
  
  delay(1000);
  
  int present=ds.reset();
  ds.select(owAddr[j]);
  ds.write(0xBE);
  
  for(int i=0; i<9; i++) owData[j][i]=ds.read(); 

  int16_t raw=(owData[j][1]<<8)|owData[j][0];
  byte cfg=(owData[j][4]&0x60);
  if(cfg==0x00) raw=raw&~7;
  else if(cfg==0x20) raw=raw&~3;
  else if(cfg==0x40) raw=raw&~1;
  return float(raw)/16.;
}
