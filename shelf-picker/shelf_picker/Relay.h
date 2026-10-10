#pragma once
#include <Arduino.h>
#include "Config.h"

// =============================================================
//  Relay : one relay channel, handles active-low boards
// =============================================================
class Relay {
public:
  explicit Relay(byte pin) : _pin(pin), _on(false) {}

  void begin() {
    digitalWrite(_pin, Cfg::RELAY_ACTIVE_LOW ? HIGH : LOW);   // off BEFORE output, no startup click
    pinMode(_pin, OUTPUT);
    _on = false;
  }
  void on()  { digitalWrite(_pin, Cfg::RELAY_ACTIVE_LOW ? LOW : HIGH);  _on = true;  }
  void off() { digitalWrite(_pin, Cfg::RELAY_ACTIVE_LOW ? HIGH : LOW);  _on = false; }
  bool isOn() const { return _on; }

private:
  byte _pin;
  bool _on;
};
