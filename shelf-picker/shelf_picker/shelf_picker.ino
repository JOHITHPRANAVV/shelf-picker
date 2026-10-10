/*
  Pneumatic shelf picker  (Arduino Uno)
  ---------------------------------------------------------------
  Valves
    5/3 closed-centre  : lift (600 mm cylinder), 2-channel relay module
    5/2 single coil x4 : lateral A (200 mm), lateral B (200 mm),
                         extender (100 mm), gripper, 4-channel relay module
  Sensors
    Ultrasonic         : measures the real gripper height, check 1 (right level)
    2 x FSR            : grip force on the jaws, check 2 (object really gripped)
  I/O
    I2C 16x2 LCD, Bluetooth (HC-05 style) on the hardware serial pins 0 and 1
    (unplug the Bluetooth TX/RX wires while uploading the sketch)

  Bluetooth command (end with newline):   <height> [slot]
      400        -> level 400 mm, slot 0
      600 2      -> level 600 mm, slot 2

  Library: LiquidCrystal_I2C (Frank de Brabander)
*/

#include "Config.h"
#include "Relay.h"
#include "Valve52.h"
#include "Valve53.h"
#include "FsrSensor.h"
#include "Ultrasonic.h"
#include "Display.h"
#include "BtLink.h"
#include "Gripper.h"
#include "Extender.h"
#include "LateralAxis.h"
#include "LiftAxis.h"
#include "Picker.h"

// =============================================================
//  Objects
// =============================================================
Display     lcd;
BtLink      bt;
FsrSensor   fsrA(Pin::FSR_A);
FsrSensor   fsrB(Pin::FSR_B);
Ultrasonic  sonar(Pin::US_TRIG, Pin::US_ECHO);

void onLiftTick(int mm) { lcd.showHeight(mm); }

LiftAxis    lift(Pin::LIFT_UP, Pin::LIFT_DOWN, sonar, onLiftTick);
LateralAxis lateral(Pin::LATERAL_A, Pin::LATERAL_B);
Extender    arm(Pin::EXTENDER);
Gripper     gripper(Pin::GRIPPER, fsrA, fsrB);

Picker      picker(lcd, bt, lift, lateral, arm, gripper);

// =============================================================
//  Arduino entry points
// =============================================================
void setup() {
  bt.begin(9600);                     // hardware serial, pins 0 and 1

  // Relays first so nothing twitches while the rest starts up
  lift.begin();
  lateral.begin();
  arm.begin();
  gripper.begin();

  sonar.begin();
  lcd.begin();

  picker.homeAll();
}

void loop() {
  Command cmd;
  if (bt.readCommand(cmd)) picker.run(cmd);
}
