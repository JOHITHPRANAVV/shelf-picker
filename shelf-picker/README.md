# Shelf Picker

Pneumatic shelf pick and place machine on an Arduino Uno. Height and slot come in over Bluetooth, the lift height is checked with an ultrasonic sensor, and the grip is confirmed with two FSRs.

## Hardware

| Part | Pins |
|---|---|
| 4-channel relay module (5/2 valves) | D13 lateral A, D12 lateral B, D11 extender, D10 gripper |
| 2-channel relay module (5/3 lift valve) | D8 up, D9 down |
| Ultrasonic | D6 trig, D7 echo |
| FSRs | A0, A1 |
| I2C LCD 16x2 | A4 SDA, A5 SCL (address 0x27, or 0x3F) |
| Bluetooth | pins 0 and 1 (unplug while uploading) |

Cylinders: 600 mm lift, two 200 mm lateral, 100 mm extender.

## Code layout

One header per class, all inside `shelf_picker/`:

| File | Job |
|---|---|
| `Config.h` | pin map and every tunable setting |
| `Relay.h` | one relay channel, active-low aware |
| `Valve52.h` | single coil, spring return valve |
| `Valve53.h` | closed centre valve with a coil interlock |
| `FsrSensor.h` | force sensor reading and touch check |
| `Ultrasonic.h` | distance in mm, single and averaged |
| `Display.h` | 16x2 I2C LCD screens |
| `BtLink.h` | reads `<height> [slot]` commands from Bluetooth |
| `Gripper.h` | closes until both FSRs feel force |
| `Extender.h` | 100 mm reach cylinder |
| `LateralAxis.h` | two 200 mm cylinders, slots 0 to 2 |
| `LiftAxis.h` | lift to 200/400/600 mm with ultrasonic height check |
| `Picker.h` | the full pick and place sequence and fault handling |
| `shelf_picker.ino` | creates the objects, `setup()` and `loop()` |

## Build

Needs the **LiquidCrystal I2C** library by Frank de Brabander. Open `shelf_picker/shelf_picker.ino` in the Arduino IDE, or in VS Code with the Arduino extension or `arduino-cli`.

`BENCH_MODE` in `Config.h` skips the sensor checks for relay-only bench tests. Set it to `false` for real use.

## Bluetooth command

End each line with a newline: `400` or `600 2` (height in mm, optional lateral slot 0 to 2).
