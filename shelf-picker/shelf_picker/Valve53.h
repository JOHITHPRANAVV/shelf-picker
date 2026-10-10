#pragma once
#include "Relay.h"

// =============================================================
//  Valve53 : 5/3 closed centre. One coil on = move, both off = HOLD
//  The off-before-on order means both coils can never be live together
// =============================================================
class Valve53 {
public:
  Valve53(byte pinA, byte pinB) : _a(pinA), _b(pinB) {}
  void begin()   { _a.begin(); _b.begin(); }
  void sideA()   { _b.off(); _a.on(); }
  void sideB()   { _a.off(); _b.on(); }
  void centre()  { _a.off(); _b.off(); }

private:
  Relay _a, _b;
};
