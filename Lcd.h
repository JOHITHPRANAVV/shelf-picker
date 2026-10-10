#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =============================================================
//  Display : 16x2 I2C LCD
// =============================================================
class Display {
public:
  Display() : _lcd(0x27, 16, 2) {}              // change 0x27 to 0x3F if blank

  void begin() { _lcd.init(); _lcd.backlight(); }

  void showHeight(int mm) {
    char buf[17];
    snprintf(buf, sizeof(buf), "Height %4d mm", mm);
    line(0, buf);
  }
  void showMessage(const char* msg) { line(1, msg); }

  // Idle screen, waiting for a height over Bluetooth
  void showPrompt() {
    line(0, "Enter the height");
    line(1, "200 400 600 mm");
  }

private:
  void line(byte row, const char* text) {       // pads to 16 chars, no clear() flicker
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", text);
    _lcd.setCursor(0, row);
    _lcd.print(buf);
  }
  LiquidCrystal_I2C _lcd;
};
THE DISPLAY