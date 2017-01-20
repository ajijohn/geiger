// DS1337 Class is by  http://www.seeedstudio.com and used
// in Seeeduino Stalker v3.0 for battery management(MCU power saving mode)
// & to generate timestamp for data logging. DateTime Class is a modified
// version supporting day-of-week.

// Original DateTime Class and its utility code is by Jean-Claude Wippler at JeeLabs
// http://jeelabs.net/projects/cafe/wiki/RTClib 
// Released under MIT License http://opensource.org/licenses/mit-license.php

#ifndef DS1337_H
#define DS1337_H


#define DS1337_ADDRESS	      0x68 //I2C Slave address

/* DS1337 Registers. Refer Sec 8.2 of application manual */
#define DS1337_SEC_REG        0x00  
#define DS1337_MIN_REG        0x01  
#define DS1337_HOUR_REG       0x02
#define DS1337_WDAY_REG       0x03
#define DS1337_MDAY_REG       0x04
#define DS1337_MONTH_REG      0x05
#define DS1337_YEAR_REG       0x06

#define DS1337_AL1SEC_REG     0x07
#define DS1337_AL1MIN_REG     0x08
#define DS1337_AL1HOUR_REG    0x09
#define DS1337_AL1WDAY_REG    0x0A

#define DS1337_AL2MIN_REG     0x0B
#define DS1337_AL2HOUR_REG    0x0C
#define DS1337_AL2WDAY_REG    0x0D

#define DS1337_CONTROL_REG          0x0E
#define DS1337_STATUS_REG           0x0F
#define DS1337_AGING_OFFSET_REG     0x0F
#define DS1337_TMP_UP_REG           0x11
#define DS1337_TMP_LOW_REG          0x12

#define EverySecond     0x01
#define EveryMinute     0x02
#define EveryHour       0x03




// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
    DateTime (long t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t date,
              uint8_t hour, uint8_t min, uint8_t sec, uint8_t wday);
    DateTime (const char* date, const char* time);

    uint8_t second() const      { return ss; }
    uint8_t minute() const      { return mm; } 
    uint8_t hour() const        { return hh; }
 
    uint8_t date() const        { return d; }
    uint8_t month() const       { return m; }
    uint16_t year() const       { return 2000 + yOff; }

    uint8_t dayOfWeek() const   { return wday;}  /*Su=0 Mo=1 Tu=3 We=4 Th=5 Fr=6 Sa=7 */

    // 32-bit time as seconds since 1/1/2000
    long get() const;   

protected:
    uint8_t yOff, m, d, hh, mm, ss, wday;
};

// RTC DS1337 chip connected via I2C and uses the Wire library.
// Only 24 Hour time format is supported in this implementation
class DS1337 {
public:
    uint8_t begin(void);
    uint8_t readRegister(uint8_t regaddress);
    void writeRegister(uint8_t regaddress, uint8_t value);

    void adjust(const DateTime& dt);  //Changes the date-time
    static DateTime now();            //Gets the current date-time

    //Decides the /INT pin's output setting
    //periodicity can be any of following defines: EverySecond, EveryMinute, EveryHour 
    void enableInterrupts(uint8_t periodicity);
    void enableInterrupts(uint8_t hh24, uint8_t mm,uint8_t ss);
    void disableInterrupts();
    void clearINTStatus();

protected:
    uint8_t intType, intPeriodicity, intHH24, intMM;
};

#endif


