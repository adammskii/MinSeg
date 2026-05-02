# MinSeg Project

Arduino code for the MinSeg self-balancing robot project in FRTN01.

The goal is to build a working control system for the MinSeg robot using an Arduino Mega 2560 / SainSmart Mega 2560, MPU6050 IMU, motor, and encoder.



## Project structure

```text
MinSeg/
├── MinSeg.ino        # Main Arduino sketch: setup(), loop(), timing
├── IMU.ino           # MPU6050 setup, calibration, tilt angle estimation
├── Motor.ino         # Motor control functions
├── Encoder.ino       # Encoder reading/counting
├── Controller.ino    # Control law
└── README.md         # Project instructions
```

Arduino treats all `.ino` files in this folder as one single program.  
Only `MinSeg.ino` should contain `setup()` and `loop()`.

---

# Arduino setup

## Required software

Install:

- Arduino IDE
- Git
- USB driver for the Arduino/SainSmart Mega if needed

In Arduino IDE, select:

```text
Tools → Board → Arduino Mega or Mega 2560
Tools → Processor → ATmega2560
Tools → Port → your Arduino port
```

---

## Required Arduino libraries

The IMU is an MPU6050 and uses Jeff Rowberg's `i2cdevlib`.

Download this GitHub repository:

```text
https://github.com/jrowberg/i2cdevlib
```

Click:

```text
Code → Download ZIP
```

Unzip it.

Inside the unzipped folder, find:

```text
i2cdevlib/Arduino/I2Cdev
i2cdevlib/Arduino/MPU6050
```

Copy both folders into your Arduino libraries folder.

Usually this folder is:

```text
Documents/Arduino/libraries/
```

The final structure should look like:

```text
Documents/Arduino/libraries/I2Cdev/I2Cdev.h
Documents/Arduino/libraries/MPU6050/MPU6050.h
```

Restart Arduino IDE after adding the libraries.

---

# Opening and uploading the project

Open the main `.ino` file in Arduino IDE:

```text
MinSeg.ino
```

Make sure the other `.ino` files are visible as tabs in the same Arduino window.

Then click **Upload**.

Open Serial Monitor or Serial Plotter with baud rate:

```text
115200
```

---

# Current IMU behavior

The IMU code should:

1. Connect to the MPU6050.
2. Calibrate while the MinSeg is held still.
3. Define the current upright/balance angle as zero.
4. Estimate the tilt angle using a complementary filter.
5. Print angle and angular rate over Serial.

During calibration, hold the MinSeg still at the angle that should count as upright.

---

# Git instructions

## First time: clone the project, DO THIS IN THE FOLDER WHERE YOU WANT IT TO LAND

```bash
git clone https://github.com/adammskii/MinSeg.git
cd MinSeg
```

---

## Normal workflow

Before starting work, always pull the latest version:

```bash
git pull
```

Then make your changes.

Check what changed:

```bash
git status
```

Add your changes:

```bash
git add .
```

Commit your changes with a short message:

```bash
git commit -m "Describe what you changed"
```

Example:

```bash
git commit -m "Add IMU complementary filter"
```

Push your changes to GitHub:

```bash
git push
```

So the normal workflow is:

```bash
git pull
# make changes
git status
git add .
git commit -m "Short message describing changes"
git push
```

---

# Important Git rules for the group

## Always pull before editing

Before changing code:

```bash
git pull
```

This reduces merge conflicts.

## Always push when done

After finishing a working change:

```bash
git add .
git commit -m "Describe changes"
git push
```

## Do not all edit the same file at the same time

Try to divide work:

```text
One person: IMU.ino
One person: Motor.ino
One person: Encoder.ino
One person: Controller.ino
```

This avoids annoying merge conflicts.

## Commit often, but only working steps

Good commits:

```text
Add IMU calibration
Add motor PWM test
Fix encoder direction
Add serial command parser
```

Bad commits:

```text
stuff
changes
asdf
final version
```

---

# If Git says your branch is behind

If `git push` fails and says something like:

```text
rejected - fetch first
```

Run:

```bash
git pull
```

Then push again:

```bash
git push
```

---

# If there is a merge conflict

A conflict means two people changed the same part of the same file.

Run:

```bash
git status
```

Open the conflicted file. You will see something like:

```text
<<<<<<< HEAD
your version
=======
their version
>>>>>>> main
```

Edit the file manually so only the correct final code remains.

Then run:

```bash
git add .
git commit -m "Resolve merge conflict"
git push
```

Ask the group before deleting someone else's code.

---

# Useful Git commands

Check current status:

```bash
git status
```

See commit history:

```bash
git log --oneline
```

See remote repository:

```bash
git remote -v
```

Pull latest changes:

```bash
git pull
```

Push local commits:

```bash
git push
```

---

# Windows terminal notes

If the project is on the `E:` drive, switch to it using:

```bat
E:
cd \SKOLA\MinSeg
```

Or use:

```bat
cd /d E:\SKOLA\MinSeg
```

---

# Development plan

Recommended order:

```text
1. IMU angle estimation
2. Motor PWM test
3. Encoder counting
4. Combine IMU + encoder + motor in fixed-time loop
5. Simple stabilizing controller
6. Parameter tuning
7. Serial commands and data plotting
8. Optional Bluetooth
```

Do not start with Bluetooth, GUI, or advanced controller design before the basic hardware works.

---

# Hardware checkpoints

Before trying to balance the MinSeg, verify:

```text
[ ] Code uploads to Arduino Mega
[ ] Serial Monitor works at 115200 baud
[ ] MPU6050 connects
[ ] Tilt angle is correct
[ ] Motor spins both directions
[ ] Motor can stop
[ ] Encoder counts correctly
[ ] Encoder direction matches motor direction
[ ] Fixed-period control loop runs
```

Only after these work should balancing control be tested.

---

# Notes

The current IMU implementation uses a complementary filter because it is simple and easy to debug.

Later, this can be replaced or extended with a Kalman filter / LQG structure if needed.
