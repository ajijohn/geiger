#include <SPI.h>
#include <SparkFunDS3234RTC.h> // Get library from: https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library

//////////////////////////////////
// Configurable Pin Definitions //
//////////////////////////////////
#define DS13074_CS_PIN 5 // DeadOn RTC Chip-select pin

// Comment out the line below if you want date printed before month.
// E.g. October 31, 2016: 10/31/16 vs. 31/10/16
#define PRINT_USA_DATE

void setup() {
  Serial.begin(115200);
  // enable 3V3 pin
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  delay(100);

  //   Call rtc.begin([cs]) to initialize the library
  //   The chip-select pin should be sent as the only parameter
  rtc.begin(DS13074_CS_PIN);
  
  // Or you can use the rtc.setTime(s, m, h, day, date, month, year)
  // function to explicitly set the time:
  // e.g. 7:32:16 | Monday October 31, 2016:
  rtc.setTime(0, 12, 8, 7, 25, 8, 18);  // Uncomment to manually set time
//  rtc.autoTime();
  // Update time/date values, so we can set alarms
  rtc.update();
}

void loop() {
  static int8_t lastSecond = -1;
  
  rtc.update();
  if (rtc.second() != lastSecond) {
    printTime(); // Print the new time
    lastSecond = rtc.second(); // Update lastSecond value
  }
}

void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }
  
  Serial.print(" | ");

  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
#ifdef PRINT_USA_DATE
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
#else
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
                 String(rtc.month()) + "/"); // Print month
#endif
  Serial.println(String(rtc.year()));        // Print year
}
