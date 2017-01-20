// DS1337 Class is by http://www.seeedstudio.com and used
// in Seeeduino Stalker v3.0 for battery management(MCU power saving mode)
// & to generate timestamp for data logging. DateTime Class is a modified
// version supporting day-of-week.

// Original DateTime Class and its utility code is by Jean-Claude Wippler at JeeLabs
// http://jeelabs.net/projects/cafe/wiki/RTClib 
// Released under MIT License http://opensource.org/licenses/mit-license.php

#include <Wire.h>
#include <avr/pgmspace.h>
#include "DS1337.h"
#include "Arduino.h"

#define SECONDS_PER_DAY 86400L

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed

static uint8_t const daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime (long t) {
    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t min, uint8_t sec, uint8_t wd) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = date;
    hh = hour;
    mm = min;
    ss = sec;
    wday = wd;
}

// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
DateTime::DateTime (const char* date, const char* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
        case 'J': m = date[1] == 'a' ? 1 : m = date[2] == 'n' ? 6 : 7; break;
        case 'F': m = 2; break;
        case 'A': m = date[2] == 'r' ? 4 : 8; break;
        case 'M': m = date[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

long DateTime::get() const {
    uint16_t days = date2days(yOff, m, d);
    return time2long(days, hh, mm, ss);
}

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

////////////////////////////////////////////////////////////////////////////////
// RTC DS1337 implementation

uint8_t DS1337::readRegister(uint8_t regaddress)
{
    Wire.beginTransmission(DS1337_ADDRESS);
    Wire.write((byte)regaddress);
    Wire.endTransmission();

    Wire.requestFrom(DS1337_ADDRESS, 1);
    return Wire.read();
}

void DS1337::writeRegister(uint8_t regaddress,uint8_t value)
{
    Wire.beginTransmission(DS1337_ADDRESS);
    Wire.write((byte)regaddress);
    Wire.write((byte)value);
    Wire.endTransmission();
}

uint8_t DS1337::begin(void) {

  unsigned char ctReg=0;
  ctReg |= 0b00011100; 
  writeRegister(DS1337_CONTROL_REG, ctReg);     //CONTROL Register Address
  delay(10);

  // set the clock to 24hr format  
  uint8_t hrReg = readRegister(DS1337_HOUR_REG);
  hrReg &= 0b10111111;
  writeRegister(DS1337_HOUR_REG, hrReg);       

  delay(10);

  return 1; 
}

//Adjust the time-date specified in DateTime format
//writing any non-existent time-data may interfere with normal operation of the RTC
void DS1337::adjust(const DateTime& dt) {

  Wire.beginTransmission(DS1337_ADDRESS);
  Wire.write((byte)DS1337_SEC_REG);  //beginning from SEC Register address

  Wire.write((byte)bin2bcd(dt.second())); 
  Wire.write((byte)bin2bcd(dt.minute()));
  Wire.write((byte)bin2bcd((dt.hour()) & 0b10111111)); //Make sure clock is still 24 Hour
  Wire.write((byte)dt.dayOfWeek());
  Wire.write((byte)bin2bcd(dt.date()));
  Wire.write((byte)bin2bcd(dt.month()));
  Wire.write((byte)bin2bcd(dt.year() - 2000));  
  Wire.endTransmission();

}

//Read the current time-date and return it in DateTime format
DateTime DS1337::now() {
  Wire.beginTransmission(DS1337_ADDRESS);
  Wire.write((byte)0x00);	
  Wire.endTransmission();
  
  Wire.requestFrom(DS1337_ADDRESS, 8);
  uint8_t ss = bcd2bin(Wire.read());
  uint8_t mm = bcd2bin(Wire.read());
   
  uint8_t hrreg = Wire.read();
  uint8_t hh = bcd2bin((hrreg & ~0b11000000)); //Ignore 24 Hour bit

  uint8_t wd =  Wire.read();
  uint8_t d = bcd2bin(Wire.read());
  uint8_t m = bcd2bin(Wire.read());
  uint16_t y = bcd2bin(Wire.read()) + 2000;
  
  return DateTime (y, m, d, hh, mm, ss, wd);
}

//Enable periodic interrupt at /INT pin. Supports only the level interrupt
//for consistency with other /INT interrupts. All interrupts works like single-shot counter
//Use refreshINTA() to re-enable interrupt.
void DS1337::enableInterrupts(uint8_t periodicity)
{

    unsigned char ctReg=0;
    ctReg |= 0b00011101; 
    writeRegister(DS1337_CONTROL_REG, ctReg);     //CONTROL Register Address
    
   switch(periodicity) 
   {
       case EverySecond:
       writeRegister(DS1337_AL1SEC_REG,  0b10000000 ); //set AM1
       writeRegister(DS1337_AL1MIN_REG,  0b10000000 ); //set AM2
       writeRegister(DS1337_AL1HOUR_REG, 0b10000000 ); //set AM3
       writeRegister(DS1337_AL1WDAY_REG, 0b10000000 ); //set AM4

       break;

       case EveryMinute:
       writeRegister(DS1337_AL1SEC_REG,  0b00000000 ); //Clr AM1
       writeRegister(DS1337_AL1MIN_REG,  0b10000000 ); //set AM2
       writeRegister(DS1337_AL1HOUR_REG, 0b10000000 ); //set AM3
       writeRegister(DS1337_AL1WDAY_REG, 0b10000000 ); //set AM4

       break;

       case EveryHour:
       writeRegister(DS1337_AL1SEC_REG,  0b00000000 ); //Clr AM1
       writeRegister(DS1337_AL1MIN_REG,  0b00000000 ); //Clr AM2
       writeRegister(DS1337_AL1HOUR_REG, 0b10000000 ); //Set AM3
       writeRegister(DS1337_AL1WDAY_REG, 0b10000000 ); //set AM4

       break;
   }
}

//Enable HH/MM/SS interrupt on /INTA pin. All interrupts works like single-shot counter
void DS1337::enableInterrupts(uint8_t hh24, uint8_t mm, uint8_t ss)
{
    unsigned char ctReg=0;
    ctReg |= 0b00011101; 
    writeRegister(DS1337_CONTROL_REG, ctReg);     //CONTROL Register Address

    writeRegister(DS1337_AL1SEC_REG,  0b00000000 | bin2bcd(ss) ); //Clr AM1
    writeRegister(DS1337_AL1MIN_REG,  0b00000000 | bin2bcd(mm)); //Clr AM2
    writeRegister(DS1337_AL1HOUR_REG, (0b00000000 | (bin2bcd(hh24) & 0b10111111))); //Clr AM3
    writeRegister(DS1337_AL1WDAY_REG, 0b10000000 ); //set AM4
}

//Disable Interrupts. This is equivalent to begin() method.
void DS1337::disableInterrupts()
{
    begin(); //Restore to initial value.
}

//Clears the interrrupt flag in status register. 
//This is equivalent to preparing the DS1337 /INT pin to high for MCU to get ready for recognizing the next INT0 interrupt
void DS1337::clearINTStatus()
{
    // Clear interrupt flag 
    uint8_t statusReg = readRegister(DS1337_STATUS_REG);
    statusReg &= 0b11111110;
    writeRegister(DS1337_STATUS_REG, statusReg);

}



