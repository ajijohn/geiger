/*
  This is the header file (interface file) for the Anemometer class. 

  The Anemometer class can be used to work with multiple anemometers more easily.
*/

#ifndef Anemometer_h
#define Anemometer_h

#include "Arduino.h"

class Anemometer {
  private:
    int _pin;
    int _distancePerPulse;
    int _pulses;
    int _interval;
    int _lastRecordedPulses;
    unsigned long _intervalTime;
    unsigned long _debounceDuration;
    unsigned long _lastDebounceTime;
    int _prevState;
  public:
    Anemometer(int pin, int distancePerPulse = 1, int interval = 3000, unsigned long debounceDuration = 10);
    void setup();
    void update();
    float getWindSpeedMPH();
    float getWindSpeedKMPH();
    void setInterval(int interval);
    void setDistancePerPulse(int distance);
};

#endif