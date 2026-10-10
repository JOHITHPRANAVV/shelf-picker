#!/usr/bin/env bash
# Run once from the repo root. One commit per class, in dependency order.
set -e
git init
git branch -M main

git add .gitignore README.md shelf_picker/Config.h
git commit -m "Add pin map and tunable settings"

git add shelf_picker/Relay.h
git commit -m "Add Relay class"

git add shelf_picker/Valve52.h
git commit -m "Add Valve52 class (5/2 single coil)"

git add shelf_picker/Valve53.h
git commit -m "Add Valve53 class (5/3 closed centre with interlock)"

git add shelf_picker/FsrSensor.h
git commit -m "Add FsrSensor class"

git add shelf_picker/Ultrasonic.h
git commit -m "Add Ultrasonic class"

git add shelf_picker/Display.h
git commit -m "Add Display class (I2C LCD screens)"

git add shelf_picker/BtLink.h
git commit -m "Add BtLink class (Bluetooth commands)"

git add shelf_picker/Gripper.h
git commit -m "Add Gripper class"

git add shelf_picker/Extender.h
git commit -m "Add Extender class"

git add shelf_picker/LateralAxis.h
git commit -m "Add LateralAxis class"

git add shelf_picker/LiftAxis.h
git commit -m "Add LiftAxis class with ultrasonic height check"

git add shelf_picker/Picker.h
git commit -m "Add Picker class (pick and place sequence)"

git add shelf_picker/shelf_picker.ino
git commit -m "Add sketch entry point"

echo "Done. Now: git remote add origin <your-repo-url> && git push -u origin main"
