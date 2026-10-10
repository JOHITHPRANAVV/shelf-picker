#pragma once
#include <Arduino.h>


// =============================================================
//  PINS
// =============================================================
namespace Pin {
  // 4-channel relay module (5/2 valves), IN1 to IN4 on pins 13 down to 10
  const byte LATERAL_A = 13;
  const byte LATERAL_B = 12;
  const byte EXTENDER  = 11;
  const byte GRIPPER   = 10;
  // 2-channel relay module (5/3 valve, two coils)
  const byte LIFT_UP   = 8;
  const byte LIFT_DOWN = 9;
  // Sensors
  const byte US_TRIG = 6;
  const byte US_ECHO = 7;
  const byte FSR_A   = A0;
  const byte FSR_B   = A1;
  // Bluetooth uses hardware serial: module TX -> Uno RX (0), module RX -> Uno TX (1)
}


// =============================================================
//  SETTINGS  (everything you will tune on the rig lives here)
// =============================================================
namespace Cfg {
  // BENCH MODE
  //   false = real use: the ultrasonic height check and the FSR grip check are ON
  //   true  = bench test: both checks are skipped, so the sequence runs with
  //           only the relay modules connected
  const bool BENCH_MODE = false;


  const bool RELAY_ACTIVE_LOW = true;          // most relay boards: LOW = coil on


  // Lift: time based stops, measured from the bottom stop
  const int           LEVEL_MM[3] = {200, 400, 600};
  const unsigned long LIFT_MS[3]  = {1000, 2000, 3000};
  const unsigned long HOME_MARGIN_MS = 500;    // extra push into the bottom stop


  // 5/2 cylinder stroke times
  const unsigned long LATERAL_MS = 1500;       // one 200 mm cylinder
  const unsigned long EXTEND_MS  = 800;        // 100 mm cylinder out
  const unsigned long RETRACT_MS = 800;        // 100 mm cylinder back


  // Gripper
  const int           FSR_THRESHOLD   = 300;   // analogRead above this = touching
  const unsigned long GRIP_TIMEOUT_MS = 2500;
  const unsigned long GRIP_SETTLE_MS  = 150;
  const unsigned long RELEASE_MS      = 700;


  // Ultrasonic height check:  height = reading - US_OFFSET_MM
  const int           US_OFFSET_MM = 50;       // reading with the gripper at the bottom stop
  const int           US_TOL_MM    = 25;       // allowed error at each stop
  const unsigned long US_SETTLE_MS = 300;      // let the carriage stop swinging first


  const byte MAX_SLOT = 2;                     // 0, 1 or 2 lateral cylinders out
}
