#pragma once
#include "Config.h"
#include "Valve52.h"

// =============================================================
//  Extender : 100 mm cylinder, reaches into the shelf
// =============================================================
class Extender {
public:
  explicit Extender(byte pin) : _valve(pin) {}
  void begin()      { _valve.begin(); }
  void extend()     { _valve.energise(); delay(Cfg::EXTEND_MS); }
  void retract()    { _valve.release();  delay(Cfg::RETRACT_MS); }
  void deenergise() { _valve.release(); }

private:
  Valve52 _valve;
};
