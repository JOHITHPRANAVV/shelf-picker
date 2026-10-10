#pragma once
#include <Arduino.h>

// =============================================================
//  BtLink : line based commands over Bluetooth
// =============================================================
struct Command {
  int heightMm;
  int slot;
};

class BtLink {
public:
  BtLink() : _len(0) {}

  void begin(long baud = 9600) { Serial.begin(baud); }

  bool readCommand(Command& cmd) {
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (_len == 0) continue;
        _buf[_len] = '\0';
        _len = 0;
        int h = 0, s = 0;
        int n = sscanf(_buf, "%d %d", &h, &s);
        if (n >= 1) {
          cmd.heightMm = h;
          cmd.slot = (n >= 2) ? s : 0;
          return true;
        }
        send("ERR format: <height> [slot]");
      } else if (_len < sizeof(_buf) - 1) {
        _buf[_len++] = c;
      }
    }
    return false;
  }

  void send(const char* msg) { Serial.println(msg); }

private:
  char _buf[16];
  byte _len;
};
