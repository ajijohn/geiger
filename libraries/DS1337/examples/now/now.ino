// Date and time functions using DS1337 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "DS1337.h"

char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

DS1337 RTC; //Create the DS1337 object

void setup () 
{
    Serial.begin(57600);
    Wire.begin();
    RTC.begin();
}

void loop () 
{
    DateTime now = RTC.now(); //get the current date-time
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.date(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(' ');
    Serial.print(weekDay[now.dayOfWeek()]);
    Serial.println();
    Serial.print("Seconds since 1/1/2000: "); 
    //print time stamp as 'long'. 
    Serial.print(now.get(),DEC);
    //This is useful token for datalogging 
    Serial.println();
    delay(1000);
}
