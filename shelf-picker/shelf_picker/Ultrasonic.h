#pragma once
#include <Arduino.h>

// =============================================================
//  Ultrasonic : HC-SR04 style, returns millimetres (-1 = no echo)
// =============================================================
class Ultrasonic {
public:
  Ultrasonic(byte trig, byte echo) : _trig(trig), _echo(echo) {}

  void begin() {
    pinMode(_trig, OUTPUT);
    pinMode(_echo, INPUT);
    digitalWrite(_trig, LOW);
  }

  int readOnceMm() {
    digitalWrite(_trig, LOW);  delayMicroseconds(2);
    digitalWrite(_trig, HIGH); delayMicroseconds(10);
    digitalWrite(_trig, LOW);
    unsigned long us = pulseIn(_echo, HIGH, 25000UL);          // ~4 m limit
    if (us == 0) return -1;
    return (int)(us * 0.343f / 2.0f);
  }

  // Average of 5 samples, needs at least 3 valid ones
  int readMm() {
    long sum = 0; byte valid = 0;
    for (byte i = 0; i < 5; i++) {
      int d = readOnceMm();
      if (d > 0) { sum += d; valid++; }
      delay(30);
    }
    return (valid >= 3) ? (int)(sum / valid) : -1;
  }

private:
  byte _trig, _echo;
};
