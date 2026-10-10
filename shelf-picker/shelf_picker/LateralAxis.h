#pragma once
#include "Config.h"
#include "Valve52.h"

// =============================================================
//  LateralAxis : two 200 mm cylinders along the shelf
//  Assumed in tandem: slot 0 = both in, 1 = A out (200 mm), 2 = A and B out (400 mm)
// =============================================================
class LateralAxis {
public:
  LateralAxis(byte pinA, byte pinB) : _a(pinA), _b(pinB), _slot(0) {}

  void begin() { _a.begin(); _b.begin(); }

  void goToSlot(byte slot) {
    if (slot == _slot) return;
    if (slot >= 1) _a.energise(); else _a.release();
    if (slot >= 2) _b.energise(); else _b.release();
    delay(Cfg::LATERAL_MS);
    _slot = slot;
  }

  void deenergise() { _a.release(); _b.release(); }

private:
  Valve52 _a, _b;
  byte _slot;
};
