## Firmware for Teensy 4.1 Microcontroller

The firmware presented here is a self-contained Arduino project. It performs
the autoalignment task by itself at the request of a host PC.


### To Compile

The most straight-forward way of setting up the development environment is to
install the [Arduino IDE with teensy packages](https://www.pjrc.com/teensy/td_download.html).
Another way is to install the [`arduino-cli`](https://arduino.github.io/arduino-cli/0.35/)
and manually add the support of Teensy to it (see [the tutorial here](
https://arduino.github.io/arduino-cli/0.35/getting-started/#adding-3rd-party-cores) ).

The code makes use of FreeRTOS for task management, as described in the
manuscript. To compile, it is necessary to install the Arduino package
[`tsandmann/freertos-teensy`](https://github.com/tsandmann/freertos-teensy)
first.

Assuming `arduino-cli` is installed, this can be done by running
```
arduino-cli lib install --git-url https://github.com/tsandmann/freertos-teensy
```
To compile, go the `src/` folder and run
```
arduino-cli compile --build-path build --fqbn teensy:avr:teensy41 main
```

The compiled hex file will be outputted into `build/` folder.

Note that the upload functionality provided in Arduino is not very mature. A
more reliable way is to download
[`teensy_loader_cli`](https://www.pjrc.com/teensy/loader_cli.html) to upload
the hex file to the Teensy:
```
./teensy_loader_cli --mcu=TEENSY41 -s -v build/main.ino.hex
```

### Descriptions For Each File

- `BoardConnection.h`: Describing the pin connections and other hardware
  setups, including the PSD calibration information.
- `PositionMotionMatrices.h`: Storing the A-matrix calibration.
- `main.ino`: Initializing all tasks, interfacing the serial interface and
  dispatching control calls.
- `BeamDetection.cpp`: Reading PSD data, converting to position information.
-  `BeamPositionControl.cpp`: Calculating the motor movement needed to move
   from the current beam position to the desired beam position.
- `MotorControl.cpp`: Controlling the motors.
- `PeakDetection.cpp`: Algorithms to detect the peaks from the scanned cavity
  signals. This is part optional and can be disabled in `main.ino`.


### Before Use

Calibrations for the PSDs and the A-matrix are required for successful
operation. Those calibrations should be performed as instructed in
`calibration/` and the results should be stored in `BoardConnection.h`
and `PositionMotionMatrices.h`.


### Control Interface

When the microcontroller is running, it listens to the serial port for incoming
instructions:
- `pp`: Print beam position, in PSD units.
- `m {n} {step}`: Move the `n`-th motor with number of steps specified by
  `{step}`.
- `mp {x1} {x2} {y1} {y2}`: Move the beam to position specified by (`x1`, `y1`,
  `x2`, `y2`). This is the very command to achieve "autoalignment".
- `unmute`: Start PSD position logging. Useful when running calibrations.
- `mute`: Stop PSD position logging.
