#pragma once
#include "Relay.h"


// =============================================================
//  Valve52 : 5/2 single coil, spring return. on = out, off = back
// =============================================================
class Valve52 {
public:
  explicit Valve52(byte pin) : _coil(pin) {}
  void begin()      { _coil.begin(); }
  void energise()   { _coil.on(); }
  void release()    { _coil.off(); }
  bool isEnergised() const { return _coil.isOn(); }


private:
  Relay _coil;
};
