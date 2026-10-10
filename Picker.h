#pragma once
#include "Config.h"
#include "Display.h"
#include "BtLink.h"
#include "LiftAxis.h"
#include "LateralAxis.h"
#include "Extender.h"
#include "Gripper.h"


// =============================================================
//  Picker : the pick and place sequence, owns no hardware
// =============================================================
class Picker {
public:
  Picker(Display& lcd, BtLink& bt, LiftAxis& lift, LateralAxis& lateral,
         Extender& arm, Gripper& grip)
    : _lcd(lcd), _bt(bt), _lift(lift), _lateral(lateral),
      _arm(arm), _grip(grip) {}


  void homeAll() {
    step("Homing");
    _grip.open();
    _arm.retract();
    _lateral.goToSlot(0);
    _lift.homeFromUnknown();
    if (!_lift.verifyHeight(0)) fault("Home height err");
    ready();
  }


  void ready() { _lcd.showPrompt(); }


  void run(const Command& cmd) {
    int lvl = levelIndex(cmd.heightMm);
    if (lvl < 0 || cmd.slot < 0 || cmd.slot > Cfg::MAX_SLOT) {
      _bt.send("ERR bad height or slot");
      _lcd.showMessage("Bad command");
      delay(1000);
      ready();
      return;
    }


    char msg[17];
    snprintf(msg, sizeof(msg), "Lifting to %d", Cfg::LEVEL_MM[lvl]);
    step(msg);               _lift.goToLevel(lvl);        // live height updates on line 0 of the LCD


    step("Checking height");                                  // verification 1: ultrasonic
    if (!_lift.verifyHeight(Cfg::LEVEL_MM[lvl])) fault("Height error");


    step("Positioning");     _lateral.goToSlot(cmd.slot);


    step("Approaching");     _arm.extend();


    step("Gripping");                                         // verification 2: FSR pair
    if (!_grip.grip()) { _arm.retract(); fault("Grip missed"); }


    step("Retracting");      _arm.retract();
    if (!_grip.holding()) fault("Dropped");


    step("Returning");       _lateral.goToSlot(0);
                             _lift.home();


    step("Releasing");       _grip.open();


    step("Done");
    _bt.send("OK done");
    delay(1500);
    ready();
  }


private:
  int levelIndex(int mm) const {
    for (byte i = 0; i < 3; i++) if (Cfg::LEVEL_MM[i] == mm) return i;
    return -1;
  }


  void step(const char* name) {
    _lcd.showMessage(name);
    if (Cfg::BENCH_MODE) _bt.send(name);              // follow the steps on your phone
  }


  // Safe state: lift holds (closed centre), every 5/2 spring returns.
  // Note that this opens the gripper, so a held object is dropped.
  void fault(const char* msg) {
    _lift.stop();
    _lateral.deenergise();
    _arm.deenergise();
    _grip.deenergise();
    _lcd.showMessage(msg);
    _bt.send(msg);
    while (true) {}                                           // reset the board to recover
  }


  Display&     _lcd;
  BtLink&      _bt;
  LiftAxis&    _lift;
  LateralAxis& _lateral;
  Extender&    _arm;
  Gripper&     _grip;
};