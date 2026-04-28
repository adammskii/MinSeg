# MinSeg Project

Arduino code for the MinSeg self-balancing robot project.

## Project structure

```text
MinSeg_main/
├── MinSeg_main.ino   # setup(), loop(), timing, program flow
├── IMU.ino           # MPU6050 setup, calibration, tilt angle estimation
├── Motor.ino         # motor control functions
├── Encoder.ino       # encoder reading/counting
└── Controller.ino    # control law
```

## Requirements

- Arduino IDE
- Arduino Mega 2560 / SainSmart Mega 2560
- I2Cdev library
- MPU6050 library

The required IMU libraries are from Jeff Rowberg's `i2cdevlib` repository:

```text
i2cdevlib/Arduino/I2Cdev
i2cdevlib/Arduino/MPU6050
```

Copy both folders into your Arduino libraries folder, for example:

```text
Documents/Arduino/libraries/I2Cdev
Documents/Arduino/libraries/MPU6050
```

Restart Arduino IDE after adding the libraries.

## Opening the project

Open `MinSeg_main.ino` in Arduino IDE.

Select:

```text
Tools → Board → Arduino Mega or Mega 2560
Tools → Port → your Arduino port
```

Then upload.

---

# Git usage

## First-time setup

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/MinSeg.git
cd MinSeg
```

## Get latest changes

Before starting work, always pull the latest version:

```bash
git pull
```

## Save your changes locally

Check what files changed:

```bash
git status
```

Add changed files:

```bash
git add .
```

Commit your changes:

```bash
git commit -m "Describe what you changed"
```

Example:

```bash
git commit -m "Add IMU complementary filter"
```

## Upload changes to GitHub

Push your commits:

```bash
git push
```

## Normal workflow

Each time you work on the project:

```bash
git pull
# make changes
git status
git add .
git commit -m "Short message describing changes"
git push
```

## If Git says there are conflicts

This means two people changed the same part of a file.

First run:

```bash
git pull
```

Open the conflicted files and look for markers like:

```text
<<<<<<< HEAD
your version
=======
their version
>>>>>>> branch-name
```

Edit the file manually so it contains the correct final code.

Then run:

```bash
git add .
git commit -m "Resolve merge conflict"
git push
```
