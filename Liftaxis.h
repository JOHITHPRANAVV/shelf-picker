#pragma once
#include "Config.h"
#include "Valve53.h"
#include "Ultrasonic.h"


// =============================================================
//  LiftAxis : 600 mm cylinder on the 5/3 valve, time based stops
// =============================================================
typedef void (*TickFn)(int heightMm);


class LiftAxis {
public:
  LiftAxis(byte pinUp, byte pinDown, Ultrasonic& sonar, TickFn onTick = nullptr)
    : _valve(pinUp, pinDown), _sonar(sonar), _onTick(onTick), _posMs(0), _mm(0) {}


  void begin() { _valve.begin(); }


  int heightMm() const { return _mm; }


  // Real height from the ultrasonic sensor, -1 if there was no echo
  int measuredMm() {
    int d = _sonar.readMm();
    return (d < 0) ? -1 : d - Cfg::US_OFFSET_MM;
  }


  // After a move: is the gripper really where we think it is?
  bool verifyHeight(int expectedMm) {
    if (Cfg::BENCH_MODE) return true;                 // no sensor on the bench
    delay(Cfg::US_SETTLE_MS);
    int h = measuredMm();
    if (h < 0) return false;
    _mm = h;                                          // show the measured value
    tick();
    return abs(h - expectedMm) <= Cfg::US_TOL_MM;
  }


  void goToLevel(byte level) {
    unsigned long target = Cfg::LIFT_MS[level];
    if (target > _posMs)      run(true,  target - _posMs);
    else if (target < _posMs) run(false, _posMs - target);
    _posMs = target;
    _mm = Cfg::LEVEL_MM[level];
    tick();
  }


  // Drive into the bottom stop. Margin makes sure we actually arrive
  void home() {
    run(false, _posMs + Cfg::HOME_MARGIN_MS);
    _posMs = 0;
    _mm = 0;
    tick();
  }


  // Power-up: position unknown, so assume the worst case (top)
  void homeFromUnknown() {
    _posMs = Cfg::LIFT_MS[2];
    _mm = Cfg::LEVEL_MM[2];
    home();
  }


  void stop() { _valve.centre(); }


private:
  void run(bool up, unsigned long ms) {
    unsigned long start = _posMs, t0 = millis(), el;
    if (up) _valve.sideA(); else _valve.sideB();
    while ((el = millis() - t0) < ms) {
      unsigned long cur = up ? start + el : (el >= start ? 0 : start - el);
      _mm = msToMm(cur);                              // time based estimate
      if (!Cfg::BENCH_MODE) {                         // with the sensor, show the real height
        int d = _sonar.readOnceMm();
        if (d > 0) _mm = max(0, d - Cfg::US_OFFSET_MM);
      }
      tick();
      delay(50);
    }
    _valve.centre();                                  // HOLD
  }


  // Piecewise linear: 0 -> 200 -> 400 -> 600 mm through the measured stop times
  int msToMm(unsigned long ms) const {
    if (ms >= Cfg::LIFT_MS[2]) return Cfg::LEVEL_MM[2];
    long x0 = 0, y0 = 0;
    for (byte i = 0; i < 3; i++) {
      long x1 = Cfg::LIFT_MS[i], y1 = Cfg::LEVEL_MM[i];
      if ((long)ms <= x1) return (int)(y0 + (y1 - y0) * ((long)ms - x0) / (x1 - x0));
      x0 = x1; y0 = y1;
    }
    return Cfg::LEVEL_MM[2];
  }


  void tick() { if (_onTick) _onTick(_mm); }


  Valve53 _valve;
  Ultrasonic& _sonar;
  TickFn _onTick;
  unsigned long _posMs;
  int _mm;
};
