#pragma once
#include "Config.h"
#include "Valve52.h"
#include "FsrSensor.h"

// =============================================================
//  Gripper : 5/2 valve + two FSRs. Coil on = closed
// =============================================================
class Gripper {
public:
  Gripper(byte pin, const FsrSensor& a, const FsrSensor& b)
    : _valve(pin), _a(a), _b(b) {}

  void begin() { _valve.begin(); }

  bool holding() const { return Cfg::BENCH_MODE || (_a.pressed() && _b.pressed()); }

  // Close until BOTH jaws feel force. Opens again and returns false on timeout
  bool grip() {
    _valve.energise();
    if (Cfg::BENCH_MODE) { delay(1000); return true; }   // no FSRs: just show the relay
    unsigned long t0 = millis();
    while (millis() - t0 < Cfg::GRIP_TIMEOUT_MS) {
      if (holding()) { delay(Cfg::GRIP_SETTLE_MS); return true; }
    }
    _valve.release();
    return false;
  }

  void open() { _valve.release(); delay(Cfg::RELEASE_MS); }
  void deenergise() { _valve.release(); }

  int forceA() const { return _a.read(); }
  int forceB() const { return _b.read(); }

private:
  Valve52 _valve;
  const FsrSensor& _a;
  const FsrSensor& _b;
};
