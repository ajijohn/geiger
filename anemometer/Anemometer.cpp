/*
  This is the implementation file for the Anemometer class.
*/

#include "Arduino.h"
#include "Anemometer.h"

/*
  Constructor for a new Anemometer object.

  pin: GPIO pin for the data line of the anemometer.
  distancePerPulse: distance traveled with one revolution of the anemometer. Defaults to 1 meter.
  interval: how long of an interval to measure pulses. The _pulses private member variable will reset to 0 each time the interval has elapsed. Defaults to 3000 milliseconds.
  debounceDuration: how long of an interval to ignore rapid changes in state due to the mechanical switch of the anemometer. Defaults to 10 milliseconds.
*/

Anemometer::Anemometer(int pin, int distancePerPulse = 1, int interval = 3000, unsigned long debounceDuration = 10) {
  _pin = pin;
  _distancePerPulse = distancePerPulse;
  _interval = interval;
  _debounceDuration = debounceDuration;
  _prevState = HIGH;
  _intervalTime = 0;
}






/*
  Initial setup to tell Arduino to use internal pullup resistors to ensure the GPIO pin is normally HIGH. 
  Invoke this function in the Arduino setup function.
*/

void Anemometer::setup() {
  pinMode(_pin, INPUT_PULLUP);
}






/*
  Update the state of the anemometer. Invoke this function in the Arduino loop function.
*/

void Anemometer::update() {
  int currentState = digitalRead(_pin);

  if (currentState != _prevState) {
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > _debounceDuration) {
    if (currentState != _prevState && currentState == LOW) {
      _pulses++;
    }
  }

  _prevState = currentState;

  if (millis() - _intervalTime > _interval) {
    _intervalTime = millis();
    _lastRecordedPulses = _pulses;
    _pulses = 0;
  }
}







/*
  Returns the wind speed in kilometers per hour.
  Note on calculation: _interval is in milliseconds, and _distancePerPulse is in meters.
  Because we want to get to km per hour, the 1000's cancel each other out.
  i.e., the full calculation would be:

  return (_lastRecordedPulses * _distancePerPulse) / (_interval / 1000) * 3600 * 1000;
*/

float Anemometer::getWindSpeedKMPH() {
  return (_lastRecordedPulses * _distancePerPulse) / _interval * 3600;
}






/*
  Returns the wind speed in miles per hour.
*/

float Anemometer::getWindSpeedMPH() {
  return (_lastRecordedPulses * _distancePerPulse) / (_interval / 1000) * 3600 / 1609.34;
}






void Anemometer::setInterval(int interval) {
  _interval = interval;
}







void setDistancePerPulse(int distance) {
  _distancePerPulse = distance;
};