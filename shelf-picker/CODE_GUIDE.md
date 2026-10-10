# Shelf Picker Code Guide

A simple guide to every keyword in the code, sorted by component. Read it top to bottom once, then use it as a lookup page.

This guide matches the current project in `shelf_picker/` (hardware serial Bluetooth, `BENCH_MODE`, idle prompt on the LCD).

---

## 1. The big picture

The code is built like a small company.

- **Workers** are small classes that each control one real part: a relay, a valve, a sensor, the LCD.
- **Managers** are classes that use several workers to move one axis: `LiftAxis`, `LateralAxis`, `Extender`, `Gripper`.
- **The boss** is `Picker`. It does not touch any pin. It only tells the managers what to do, in the right order.
- **The main file** `shelf_picker.ino` creates all the objects and runs `setup()` and `loop()`.

Who uses who:

```
shelf_picker.ino
 └─ Picker  (the boss, runs the pick sequence)
     ├─ Display      ->  I2C LCD
     ├─ BtLink       ->  Bluetooth (Serial)
     ├─ LiftAxis     ->  Valve53 -> Relay x2     + Ultrasonic
     ├─ LateralAxis  ->  Valve52 x2 -> Relay x2
     ├─ Extender     ->  Valve52 -> Relay
     └─ Gripper      ->  Valve52 -> Relay        + FsrSensor x2
```

The files:

| File | What is inside |
|---|---|
| `Config.h` | all pin numbers and all tunable numbers |
| `Relay.h` | `Relay` class |
| `Valve52.h` | `Valve52` class |
| `Valve53.h` | `Valve53` class |
| `FsrSensor.h` | `FsrSensor` class |
| `Ultrasonic.h` | `Ultrasonic` class |
| `Display.h` | `Display` class |
| `BtLink.h` | `Command` struct and `BtLink` class |
| `Gripper.h` | `Gripper` class |
| `Extender.h` | `Extender` class |
| `LateralAxis.h` | `LateralAxis` class |
| `LiftAxis.h` | `LiftAxis` class |
| `Picker.h` | `Picker` class |
| `shelf_picker.ino` | objects, `setup()`, `loop()` |

---

## 2. Basic C++ words used in this code

### Files and setup words

| Word | Simple meaning | Example |
|---|---|---|
| `#include` | Copy another file in here so I can use what is inside | `#include "Relay.h"` |
| `#include <...>` | Same, but from a library or Arduino itself | `#include <Wire.h>` |
| `#include "..."` | Same, but from my own project folder | `#include "Config.h"` |
| `#pragma once` | Only load this file one time, even if many files include it | top of every `.h` file |
| `//` | A comment. The computer ignores the rest of the line | `// this is a note` |
| `/* ... */` | A comment over many lines | the box at the top of the sketch |

### Names and groups

| Word | Simple meaning | Example |
|---|---|---|
| `namespace` | A named box that holds related things, so names do not clash | `namespace Pin { ... }` |
| `::` | "Look inside that box" | `Pin::LIFT_UP`, `Cfg::LIFT_MS` |
| `_name` | Our habit: an underscore at the start means "this belongs inside the class" | `_pin`, `_valve` |

### Boxes that hold numbers (variables)

| Type | What it holds | Size and range |
|---|---|---|
| `bool` | true or false | 1 byte |
| `byte` | whole number 0 to 255 | good for pin numbers |
| `char` | one letter or symbol | `'a'`, `'\n'` |
| `int` | whole number, can be negative | -32768 to 32767 |
| `long` | bigger whole number, can be negative | about plus or minus 2 billion |
| `unsigned long` | big whole number, never negative | 0 to about 4 billion, used for time in ms |
| `float` | number with a decimal point | `0.343f` |

Making one:

```cpp
const byte LIFT_UP = 8;        // a fixed number called LIFT_UP
int heightMm = 0;              // a number we can change later
```

| Word | Simple meaning |
|---|---|
| `const` | This will never change. The computer will complain if we try |
| `[3]` | An array, a row of 3 boxes, like `{200, 400, 600}` |
| `array[0]` | The first box. Counting starts at 0, so `LEVEL_MM[2]` is the third one (600) |

### Making decisions and repeating

| Word | Simple meaning | Example |
|---|---|---|
| `if (...) { }` | Do this only when the condition is true | `if (slot == _slot) return;` |
| `else` | Otherwise do this | `if (up) ...; else ...;` |
| `while (...) { }` | Keep repeating as long as the condition is true | waiting for the grip |
| `for (...) { }` | Repeat a fixed number of times | the 5 ultrasonic readings |
| `return` | Stop this function and give an answer back | `return true;` |
| `continue` | Skip the rest of this round and go to the next one | skip empty lines in Bluetooth |
| `while (true) {}` | Freeze forever (used in `fault`) | reset the board to leave it |

Symbols used inside conditions:

| Symbol | Meaning |
|---|---|
| `==` | is equal to (two equals signs, one `=` stores a value) |
| `!=` | is not equal to |
| `<` `>` `<=` `>=` | smaller, bigger, smaller or equal, bigger or equal |
| `&&` | AND, both must be true |
| `\|\|` | OR, at least one must be true |
| `!` | NOT, flips true and false |
| `a ? b : c` | Short if. If `a` is true, use `b`, else use `c` |
| `+=` `++` | add to it, add one |

The short if is used all over the code. This line means "if the relay board is active low, write HIGH, otherwise write LOW":

```cpp
digitalWrite(_pin, Cfg::RELAY_ACTIVE_LOW ? HIGH : LOW);
```

### Functions

A function is a named set of steps.

```cpp
int add(int a, int b) {     // answer type, name, inputs
  return a + b;             // the answer
}
```

| Word | Simple meaning |
|---|---|
| `void` | This function gives back no answer, it just does things |
| `bool`, `int` (before a function name) | The type of answer it gives back |
| `(int mm)` | The input the function needs |
| `byte level = 0` in the input list | A default value, used if nobody gives one |

### Classes (the most important part)

A class is a plan for one kind of thing. An object is one real thing made from that plan.

```cpp
class Relay {                     // the plan
public:                           // anyone can use these
  Relay(byte pin);                // constructor, runs when the object is made
  void on();                      // a function inside the class
private:                          // only the class itself can use these
  byte _pin;                      // a variable inside the class
};

Relay lamp(13);                   // an object made from the plan
lamp.on();                        // use it with a dot
```

| Word | Simple meaning |
|---|---|
| `class` | A plan that bundles variables and functions together |
| `public:` | Parts that outside code may use |
| `private:` | Parts that only the class itself may use |
| Constructor | Same name as the class, no answer type. Sets the object up |
| `: _pin(pin), _on(false)` | The "start list" after a constructor. Sets the starting values |
| `explicit` | Stops the computer from guessing a conversion. Safe to ignore |
| `const` after a function `bool isOn() const` | Promise: this function only looks, it never changes anything |
| `.` | Use something of an object: `lamp.on()` |
| `struct` | Like a class, but only a bag of variables (see `Command`) |
| `typedef` | Give a long type a short name (see `TickFn`) |
| `nullptr` | "Points to nothing" |

### References and pointers

| Word | Simple meaning |
|---|---|
| `Display& lcd` | A reference. Not a copy: the real `lcd`. Changes affect the original |
| `const FsrSensor& a` | Same, but we promise only to read it |
| `Relay _coil;` inside a class | A part living inside the object, like an engine inside a car |
| `void (*TickFn)(int)` | A "function holder". It can hold the address of a function to call later |

### Small helpers

| Word | Simple meaning |
|---|---|
| `(int)x` | Force `x` to become an int. Drops the decimals |
| `sizeof(_buf)` | How many bytes the variable takes |
| `'\0'` | The invisible end-of-text mark |
| `"..."` | Text. Always ends with a hidden `'\0'` |
| `'a'` | One letter |
| `0x27` | A number written in hex. 0x27 is 39 |
| `1000UL` or `25000UL` | The `UL` says "this is an unsigned long" |

---

## 3. Arduino words used in this code

| Word | What it does | Syntax |
|---|---|---|
| `setup()` | Runs one time when the board starts | `void setup() { ... }` |
| `loop()` | Runs again and again forever | `void loop() { ... }` |
| `pinMode` | Says if a pin sends out or reads in | `pinMode(8, OUTPUT);` |
| `OUTPUT` / `INPUT` | The two pin modes used here | |
| `digitalWrite` | Sets a pin to HIGH (5 V) or LOW (0 V) | `digitalWrite(8, HIGH);` |
| `HIGH` / `LOW` | Pin on (5 V) or off (0 V) | |
| `analogRead` | Reads a pin as a number from 0 to 1023 | `int v = analogRead(A0);` |
| `A0`, `A1` | Names of the analog input pins | |
| `delay(ms)` | Wait for that many milliseconds. Nothing else happens | `delay(500);` |
| `delayMicroseconds(us)` | Wait for that many millionths of a second | `delayMicroseconds(10);` |
| `millis()` | Milliseconds since the board started. Used as a stopwatch | `unsigned long t0 = millis();` |
| `pulseIn` | Measures how long a pin stays HIGH (in microseconds) | `pulseIn(7, HIGH, 25000);` |
| `abs(x)` | Remove the minus sign | `abs(-5)` gives 5 |
| `max(a, b)` | The bigger of two numbers | `max(0, d - 50)` |
| `snprintf` | Writes a formatted text into a char box safely | see below |
| `sscanf` | Reads numbers out of a text | see below |

### The stopwatch pattern

Used in `Gripper::grip` and `LiftAxis::run`:

```cpp
unsigned long t0 = millis();             // note the start time
while (millis() - t0 < 2500) {           // while less than 2.5 seconds have passed
  // keep checking something
}
```

### Text format codes (`snprintf`)

```cpp
char buf[17];
snprintf(buf, sizeof(buf), "Height %4d mm", 235);   // buf = "Height  235 mm"
```

| Code | Meaning |
|---|---|
| `%d` | put a whole number here |
| `%4d` | a whole number, at least 4 characters wide (pads with spaces) |
| `%-16s` | text, at least 16 characters wide, text on the left (pads on the right) |

Why we pad to 16: the LCD is 16 characters wide. Padding wipes old letters, so we never need `clear()`, which makes the screen flicker.

### Reading numbers from text (`sscanf`)

```cpp
int h = 0, s = 0;
int n = sscanf("600 2", "%d %d", &h, &s);   // h = 600, s = 2, n = 2 (two numbers found)
```

`&h` means "put the answer into the variable `h`". If only one number is found, `n` is 1.

---

## 4. Components, one by one

For each component: what it is, the basic syntax to use the real part, the keywords it needs, and what our class does.

---

### 4.1 Relay (and the two relay modules)

**What it is.** An electric switch controlled by a small signal. The Arduino pin cannot power a valve coil directly, so the pin tells a relay, and the relay switches the coil's power.

**Our setup.** Four-channel module for the four 5/2 valves, two-channel module for the 5/3 valve.

**Active low.** Most relay boards turn ON when the pin is LOW and OFF when it is HIGH. That is why `RELAY_ACTIVE_LOW = true`.

**Basic syntax (no class):**

```cpp
digitalWrite(8, HIGH);   // relay off (active low board) BEFORE making it an output
pinMode(8, OUTPUT);
digitalWrite(8, LOW);    // relay ON, LED lights, you hear a click
digitalWrite(8, HIGH);   // relay OFF
```

Setting HIGH first and then `pinMode` stops a random click at power-up.

**Our class `Relay`:**

| Function | What it does |
|---|---|
| `Relay(pin)` | Remembers which pin this relay is on |
| `begin()` | Starts with the relay off, then makes the pin an output |
| `on()` | Switch relay on |
| `off()` | Switch relay off |
| `isOn()` | Says if we last turned it on |

---

### 4.2 Valve52 (5/2 valve with one coil)

**What it is.** A valve with 5 ports and 2 positions. One coil pushes it to one position, a spring pushes it back when the coil is off.

- Coil ON means cylinder goes OUT (or gripper CLOSES).
- Coil OFF means the spring returns it (cylinder goes back, gripper OPENS).

Used for: lateral A, lateral B, extender, gripper.

**Our class `Valve52`** (it owns one `Relay`):

| Function | What it does |
|---|---|
| `begin()` | Start the relay |
| `energise()` | Coil on |
| `release()` | Coil off, spring returns |
| `isEnergised()` | Is the coil on |

---

### 4.3 Valve53 (5/3 valve with two coils)

**What it is.** 5 ports, 3 positions. Coil A moves it one way, coil B the other way. With both coils off, it sits in the middle and is **closed centre**, so the air is blocked and the cylinder **holds its place**. That is how the lift can stop at 200, 400 and 600 mm.

Used for: the lift (600 mm cylinder).

**Safety rule.** Never turn both coils on together. Our code always turns the other coil OFF first, then turns the wanted one ON.

**Our class `Valve53`** (owns two relays):

| Function | What it does |
|---|---|
| `begin()` | Start both relays |
| `sideA()` | Coil B off, coil A on (lift goes UP) |
| `sideB()` | Coil A off, coil B on (lift goes DOWN) |
| `centre()` | Both off, HOLD position |

---

### 4.4 FSR (force sensor)

**What it is.** A pad whose resistance drops when you squeeze it. The harder the squeeze, the higher the number we read.

**Our setup.** Two FSRs on the two gripper jaws. Each one needs its own 10k resistor to ground (a "divider"), and goes to A0 and A1.

**Basic syntax:**

```cpp
int force = analogRead(A0);     // 0 = no squeeze, up to 1023 = strong squeeze
if (force > 300) { /* touching */ }
```

**Our class `FsrSensor`:**

| Function | What it does |
|---|---|
| `read()` | The raw number 0 to 1023 |
| `pressed()` | True when the number is above `FSR_THRESHOLD` |

---

### 4.5 Ultrasonic sensor (HC-SR04 style)

**What it is.** It sends a short sound beep and listens for the echo. The longer the echo takes, the farther the surface. We use it to measure the real height of the gripper.

**Pins.** TRIG (we send a pulse) on pin 6, ECHO (we listen) on pin 7.

**Basic syntax:**

```cpp
pinMode(6, OUTPUT);  pinMode(7, INPUT);

digitalWrite(6, LOW);  delayMicroseconds(2);
digitalWrite(6, HIGH); delayMicroseconds(10);       // a 10 microsecond pulse starts the beep
digitalWrite(6, LOW);

unsigned long us = pulseIn(7, HIGH, 25000);         // how long the echo pin stayed HIGH
// distance in mm = us * 0.343 / 2
```

Why `* 0.343 / 2`: sound travels 0.343 mm every microsecond, and the beep goes there and back, so we divide by 2.

`pulseIn(..., 25000)` gives up after 25 ms and returns 0, meaning "no echo".

**Our class `Ultrasonic`:**

| Function | What it does |
|---|---|
| `begin()` | Set the pin modes |
| `readOnceMm()` | One reading in mm, or -1 if no echo |
| `readMm()` | Average of 5 readings (needs at least 3 good ones), or -1. Slower but steadier |

---

### 4.6 LCD (16x2 with I2C backpack)

**What it is.** A screen with 2 rows of 16 letters. The small board on the back (the "backpack") lets us use only 2 wires: SDA and SCL.

**Pins.** SDA on A4, SCL on A5, plus 5V and GND. The blue screw on the backpack sets the contrast.

**Basic syntax (library `LiquidCrystal_I2C`):**

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);   // address, columns, rows

lcd.init();            // start it
lcd.backlight();       // light on
lcd.setCursor(0, 1);   // column 0, row 1 (second row)
lcd.print("Hello");    // write text there
```

| Word | Meaning |
|---|---|
| `0x27` | The I2C address. Some boards use `0x3F` |
| Row 0 / Row 1 | Top row / bottom row |

**Our class `Display`:**

| Function | What it does |
|---|---|
| `begin()` | Start the LCD and turn on the backlight |
| `showHeight(mm)` | Row 0: `Height  235 mm` |
| `showMessage(text)` | Row 1: the current step, like `Lifting to 400` |
| `showPrompt()` | Idle screen: `Enter the height` and `200 400 600 mm` |
| `line(row, text)` (private) | Pads text to 16 letters and writes it on that row |

---

### 4.7 Bluetooth module (HC-05 style)

**What it is.** A wireless serial cable. Your phone sends text, the module hands it to the Arduino over its serial pins.

**Wiring.** Module TX goes to Uno RX (pin 0). Module RX goes to Uno TX (pin 1). They cross. Unplug these two wires while uploading code.

**Basic syntax (`Serial`):**

```cpp
Serial.begin(9600);                 // start, 9600 = speed (baud)
if (Serial.available()) {           // is there a letter waiting?
  char c = Serial.read();           // take one letter
}
Serial.println("hello");            // send text back, with a new line
```

**Our command format.** `<height> [slot]` then a new line. Examples: `400` or `600 2`.

**Our struct `Command`:** a small bag holding the two numbers.

```cpp
struct Command {
  int heightMm;   // 200, 400 or 600
  int slot;       // 0, 1 or 2
};
```

**Our class `BtLink`:**

| Function | What it does |
|---|---|
| `begin(baud)` | Start serial at 9600 |
| `readCommand(cmd)` | Collects letters until a new line, then reads the numbers into `cmd`. Returns true when a full command arrived |
| `send(text)` | Send a text line to the phone |

How `readCommand` works, in steps:

1. Take letters one by one and store them in `_buf`.
2. When a new line (`\n` or `\r`) arrives, close the text with `'\0'`.
3. Use `sscanf` to pull out the numbers.
4. If at least one number was found, fill `cmd` and return true. If not, send an error.

---

### 4.8 Gripper (valve + two FSRs)

**What it is.** A cylinder that squeezes. Coil on = closed. The two FSRs tell us when it truly holds something.

**Our class `Gripper`:**

| Function | What it does |
|---|---|
| `begin()` | Start its valve |
| `grip()` | Close, then wait up to `GRIP_TIMEOUT_MS` until **both** FSRs feel force. Returns true if it gripped, false (and opens again) if not |
| `holding()` | True if both FSRs are pressed right now |
| `open()` | Release and wait `RELEASE_MS` |
| `deenergise()` | Coil off immediately, no waiting (for faults) |
| `forceA()`, `forceB()` | The raw FSR numbers |

In `BENCH_MODE` it skips the FSR wait so you can test with only relays.

---

### 4.9 Extender (100 mm cylinder)

**What it is.** A short cylinder that pushes the gripper into the shelf to reach the object.

| Function | What it does |
|---|---|
| `begin()` | Start its valve |
| `extend()` | Coil on, wait `EXTEND_MS` |
| `retract()` | Coil off, wait `RETRACT_MS` |
| `deenergise()` | Coil off, no waiting |

---

### 4.10 LateralAxis (two 200 mm cylinders)

**What it is.** Two cylinders that move the carriage along the shelf. We assume they are in a row (tandem), so they add up.

| Slot | Cylinder A | Cylinder B | Travel |
|---|---|---|---|
| 0 | in | in | 0 mm |
| 1 | out | in | 200 mm |
| 2 | out | out | 400 mm |

| Function | What it does |
|---|---|
| `begin()` | Start both valves |
| `goToSlot(slot)` | Sets A and B for that slot, waits `LATERAL_MS`. Does nothing if already there |
| `deenergise()` | Both coils off |

---

### 4.11 LiftAxis (600 mm cylinder, up and down)

**What it is.** The vertical cylinder on the 5/3 valve. It has **no position sensor for movement**, so it moves by **time**: run up for 2000 ms and it should reach about 400 mm. Then the ultrasonic sensor **checks** the result.

**Time to height.** `LIFT_MS` = {1000, 2000, 3000} are the times from the bottom to 200, 400, 600 mm. `msToMm` draws straight lines between these points to guess the height at any moment, so the LCD number counts up smoothly.

**Homing.** At power-up we do not know where the lift is, so we assume the top, then drive DOWN for the full time plus `HOME_MARGIN_MS`, pushing into the bottom stop. After that we know it is at 0.

| Function | What it does |
|---|---|
| `begin()` | Start the valve |
| `goToLevel(level)` | Move up or down to level 0, 1 or 2 (200, 400, 600 mm), by time |
| `home()` | Drive down into the bottom stop. Position becomes 0 |
| `homeFromUnknown()` | Same, but assume the top first. Used at power-up |
| `stop()` | Valve to centre, holds position |
| `measuredMm()` | Real height from the ultrasonic sensor, or -1 |
| `verifyHeight(expected)` | Wait `US_SETTLE_MS`, measure, true if within `US_TOL_MM` of expected |
| `heightMm()` | The height number we currently believe |
| `run(up, ms)` (private) | Drive up or down for `ms`, updating the LCD every 50 ms, then centre the valve |
| `msToMm(ms)` (private) | Turn a time into a height guess |
| `tick()` (private) | Call the "height changed" function if one was given |

**What is `TickFn onTick`?** A function we hand to the lift. Each time the height changes, the lift calls it. In `shelf_picker.ino` that function is `onLiftTick`, which shows the height on the LCD. This way `LiftAxis` does not need to know about the LCD.

```cpp
typedef void (*TickFn)(int heightMm);       // "a function that takes an int and returns nothing"
void onLiftTick(int mm) { lcd.showHeight(mm); }
```

**Two ways the live number is made:**

- `BENCH_MODE = true`: the number is a time estimate.
- `BENCH_MODE = false`: the number is the real ultrasonic reading (the lift still stops by time).

---

### 4.12 Picker (the boss)

**What it is.** The class that runs the whole job in order. It has no pins. It only calls the managers.

| Function | What it does |
|---|---|
| `homeAll()` | At power-up: open gripper, retract arm, lateral to slot 0, lift to bottom, check height, show prompt |
| `ready()` | Show the idle prompt on the LCD |
| `run(cmd)` | The full pick for one command |
| `levelIndex(mm)` (private) | Turn 200/400/600 into 0/1/2, or -1 if invalid |
| `step(name)` (private) | Show the step name on the LCD (and send it to the phone in bench mode) |
| `fault(msg)` (private) | Safe stop: lift holds, all 5/2 coils off, show the message, freeze |

**The sequence in `run`:**

| # | Step shown | What happens | Parts used |
|---|---|---|---|
| 1 | `Lifting to 400` | Lift moves to the level by time, LCD height counts up | LiftAxis |
| 2 | `Checking height` | Ultrasonic must read the right height, else `Height error` | LiftAxis, Ultrasonic |
| 3 | `Positioning` | Lateral cylinders go to the slot | LateralAxis |
| 4 | `Approaching` | Extender pushes in | Extender |
| 5 | `Gripping` | Gripper closes until both FSRs feel force, else `Grip missed` | Gripper, FSR x2 |
| 6 | `Retracting` | Extender pulls back, FSRs must still feel force, else `Dropped` | Extender, Gripper |
| 7 | `Returning` | Lateral back to slot 0, lift down to 0 | LateralAxis, LiftAxis |
| 8 | `Releasing` | Gripper opens | Gripper |
| 9 | `Done` | Tell the phone `OK done`, show the prompt again | BtLink, Display |

The two **checks** are the "double verification": step 2 (height) and step 5 (grip).

**Faults.** If a check fails, `fault()` shows the message, sends it to the phone, turns everything into the safe state, and freezes. Press the reset button to start again. Note: the safe state turns the gripper coil off, which opens the gripper, so a held object would drop.

---

## 5. Settings in `Config.h`

All the numbers you will tune are here. Change only this file for tuning.

### Pins (`namespace Pin`)

| Name | Pin | Used by |
|---|---|---|
| `LATERAL_A` | 13 | 4-channel relay IN1 |
| `LATERAL_B` | 12 | 4-channel relay IN2 |
| `EXTENDER` | 11 | 4-channel relay IN3 |
| `GRIPPER` | 10 | 4-channel relay IN4 |
| `LIFT_UP` | 8 | 2-channel relay IN1 |
| `LIFT_DOWN` | 9 | 2-channel relay IN2 |
| `US_TRIG` | 6 | ultrasonic trigger |
| `US_ECHO` | 7 | ultrasonic echo |
| `FSR_A` | A0 | left jaw FSR |
| `FSR_B` | A1 | right jaw FSR |

Bluetooth is on pins 0 and 1 (no name in the file, it uses `Serial`). The LCD is on A4 and A5 (the `Wire` library handles it).

### Tuning numbers (`namespace Cfg`)

| Name | Meaning | How to tune |
|---|---|---|
| `BENCH_MODE` | true skips ultrasonic and FSR checks | true for relay-only tests, false for real use |
| `RELAY_ACTIVE_LOW` | true if LOW turns the relay on | flip it if relays act backwards |
| `LEVEL_MM` | the three heights {200, 400, 600} | change only if levels change |
| `LIFT_MS` | time from bottom to each level {1000, 2000, 3000} | time the lift with a stopwatch |
| `HOME_MARGIN_MS` | extra down time to reach the bottom stop | raise if homing stops short |
| `LATERAL_MS` | time for a 200 mm lateral stroke | time one cylinder moving |
| `EXTEND_MS` | time for the 100 mm cylinder to go out | time it |
| `RETRACT_MS` | time for it to come back | time it |
| `FSR_THRESHOLD` | FSR number that counts as "touching" | read the number pressed and not pressed, pick a value between |
| `GRIP_TIMEOUT_MS` | how long to wait for the grip before giving up | raise if gripping is slow |
| `GRIP_SETTLE_MS` | short wait after the grip is detected | leave it |
| `RELEASE_MS` | wait time while the gripper opens | time it |
| `US_OFFSET_MM` | ultrasonic reading when the gripper is at the bottom | measure once at the bottom stop |
| `US_TOL_MM` | how far off the height may be | raise if you get false `Height error` |
| `US_SETTLE_MS` | wait for the carriage to stop shaking before measuring | raise if readings jump |
| `MAX_SLOT` | the biggest lateral slot number | 2 for two cylinders |

Height from ultrasonic: `height = reading - US_OFFSET_MM`.

---

## 6. The main file `shelf_picker.ino`

Three jobs.

**1. Make the objects.** One real thing from each class:

```cpp
Display     lcd;
BtLink      bt;
FsrSensor   fsrA(Pin::FSR_A);
FsrSensor   fsrB(Pin::FSR_B);
Ultrasonic  sonar(Pin::US_TRIG, Pin::US_ECHO);
LiftAxis    lift(Pin::LIFT_UP, Pin::LIFT_DOWN, sonar, onLiftTick);
LateralAxis lateral(Pin::LATERAL_A, Pin::LATERAL_B);
Extender    arm(Pin::EXTENDER);
Gripper     gripper(Pin::GRIPPER, fsrA, fsrB);
Picker      picker(lcd, bt, lift, lateral, arm, gripper);
```

Order matters: a part must exist before something that needs it. That is why `sonar` comes before `lift`, and `lcd` before `picker`.

**2. `setup()`** runs once:

1. Start Bluetooth.
2. Start all the relay classes first, so nothing twitches.
3. Start the ultrasonic and the LCD.
4. `picker.homeAll()`, bring everything to the home position.

**3. `loop()`** runs forever:

```cpp
void loop() {
  Command cmd;
  if (bt.readCommand(cmd)) picker.run(cmd);   // a full command arrived: run the pick
}
```

---

## 7. Follow one command from start to end

You send `400 1` on your phone.

1. `loop()` calls `bt.readCommand(cmd)`. It collects the letters `4`, `0`, `0`, ` `, `1`, then the new line.
2. `sscanf` finds two numbers. `cmd.heightMm = 400`, `cmd.slot = 1`. It returns true.
3. `picker.run(cmd)` starts. `levelIndex(400)` gives 1 (the middle level). Slot 1 is fine (0 to 2).
4. `step("Lifting to 400")` shows on the LCD. `_lift.goToLevel(1)` runs the lift up for 2000 ms. Every 50 ms `tick()` updates the LCD height.
5. `verifyHeight(400)` waits, measures, and compares with 400 plus or minus 25.
6. `_lateral.goToSlot(1)` turns on lateral A and waits.
7. `_arm.extend()` turns on the extender and waits.
8. `_grip.grip()` turns on the gripper and waits for both FSRs.
9. `_arm.retract()` pulls back, then `holding()` checks the grip.
10. `_lateral.goToSlot(0)` and `_lift.home()` bring everything back down.
11. `_grip.open()` releases. The phone gets `OK done`. The LCD returns to the prompt.

---

## 8. Quick lookup (A to Z)

| Word | Short meaning |
|---|---|
| active low | Relay is ON when the pin is LOW |
| analogRead | Read a pin as 0 to 1023 |
| array | A row of values, like `{200, 400, 600}` |
| baud | Serial speed. Both sides must match (9600) |
| bench mode | Test mode with sensor checks skipped |
| `byte` | Whole number 0 to 255 |
| class | A plan that bundles data and functions |
| closed centre | 5/3 valve that blocks the air in the middle, so the cylinder holds |
| coil | The electromagnet that moves a valve |
| `const` | Never changes |
| constructor | The setup function of a class, same name as the class |
| `delay` | Wait, and do nothing else |
| deenergise | Turn the coil off |
| energise | Turn the coil on |
| fault | Safe stop after a failed check |
| FSR | Force sensing resistor |
| homing | Driving to a known start position |
| I2C | A 2-wire way to talk to the LCD (SDA and SCL) |
| `millis` | The board's stopwatch in ms |
| namespace | A named box of related names |
| object | One real thing made from a class |
| `private` | Only the class itself can use it |
| `public` | Anyone can use it |
| reference `&` | The real thing, not a copy |
| relay | An electric switch moved by a small signal |
| slot | Lateral position 0, 1 or 2 |
| spring return | Valve returns by spring when the coil is off |
| struct | A class that is only a bag of variables |
| tandem | Two cylinders in a row, travel adds up |
| tick | The "height changed" call from lift to LCD |
| `void` | A function that gives no answer back |

---

## 9. Common mistakes

- **One `=` instead of two.** `if (x = 5)` stores 5. Use `if (x == 5)`.
- **Starting at 1 for arrays.** The first box is number 0.
- **Forgetting the new line in the Bluetooth app.** The sketch waits for it.
- **Uploading with Bluetooth wires on pins 0 and 1.** Unplug them first.
- **Both coils of the 5/3 valve on together.** `Valve53` prevents it. Do not bypass it with raw `digitalWrite`.
- **Leaving `BENCH_MODE` on.** The sensor checks are skipped, so nothing is being verified.
- **Using the Uno 5V pin for relay coils and valves.** Give them their own supply and join the grounds.
