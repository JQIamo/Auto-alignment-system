# Source Code for Autoalignment System

This git repository contains the entire source code for the autoalignment
system described in {link to the paper}. It includes:

- `src/`: The firmware for the microcontroller (Teensy 4.1).
- `scripts/`: The python scripts, run on a host PC, to communicate with the microcontroller
  for manual control and diagnostic purposes.
- `calibration/`: A Jupyter notebook that calculate the $A$ matrix from calibration data.
-  `eagle/`: Eagle schematic and board files for fabricating the main control board and the detector boards.

Instructions for how to compile and use the source code are documented in each
individual folder.
