#pragma once
#include <Arduino.h>
#include "Config.h"

enum GripResult {
  GRIP_OK,           // both FSRs pressed AND object seen by ultrasonic
  GRIP_NO_FORCE,     // FSRs not pressed (nothing grabbed)
  GRIP_FALSE_FORCE   // FSRs pressed but no object (arms touching each other)
};

// Call once from setup()
inline void gripCheckBegin() {
  pinMode(Pin::GRIP_TRIG, OUTPUT);
  pinMode(Pin::GRIP_ECHO, INPUT);
}

// One distance reading in mm, or -1 if there is no echo
inline int gripReadMM() {
  digitalWrite(Pin::GRIP_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(Pin::GRIP_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(Pin::GRIP_TRIG, LOW);

  unsigned long us = pulseIn(Pin::GRIP_ECHO, HIGH, 25000UL);
  if (us == 0) return -1;
  return (int)(us * 0.343f / 2.0f);
}

// Median of several readings (no echo counts as very far)
inline int gripReadFilteredMM() {
  int s[9];
  byte n = min(Cfg::GRIP_US_SAMPLES, (byte)9);
  for (byte i = 0; i < n; i++) {
    int d = gripReadMM();
    s[i] = (d < 0) ? 9999 : d;
    delay(30);
  }
  for (byte i = 0; i < n - 1; i++)
    for (byte j = i + 1; j < n; j++)
      if (s[j] < s[i]) { int t = s[i]; s[i] = s[j]; s[j] = t; }
  return s[n / 2];
}

inline bool gripFsrPressed() {
  return analogRead(Pin::FSR_A) > Cfg::FSR_THRESHOLD &&
         analogRead(Pin::FSR_B) > Cfg::FSR_THRESHOLD;
}

// Main function: call after the gripper has closed and settled
inline GripResult checkGrip() {
  if (Cfg::BENCH_MODE) return GRIP_OK;          // bench test: skip checks

  if (!gripFsrPressed()) return GRIP_NO_FORCE;

  int d = gripReadFilteredMM();
  if (abs(d - Cfg::GRIP_EXPECTED_MM) <= Cfg::GRIP_TOL_MM) return GRIP_OK;

  return GRIP_FALSE_FORCE;                      // force but no object in front of sensor
}