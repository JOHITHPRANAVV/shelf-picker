#pragma once
#include <Arduino.h>
#include "Config.h"

// =============================================================
//  FsrSensor : force sensor on a 10k divider
// =============================================================
class FsrSensor {
public:
  explicit FsrSensor(byte pin) : _pin(pin) {}
  int  read() const    { return analogRead(_pin); }
  bool pressed() const { return read() > Cfg::FSR_THRESHOLD; }

private:
  byte _pin;
};
