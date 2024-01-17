## Utility Scripts For Communication And Beam Visualization

The communication between the host PC and the microcontroller is achieved
through the serial connection to the microcontroller.

In order to have multiple scripts communicating with the microcontroller at the
same time, all communication are relayed through the script `comm.py`, which
talks to other scripts through ZMQ sockets.

- `comm.py`: Communication host. Dispatch the requests from other scripts to
  the microcontroller.
- `beam_visualize.py`: A GUI program, displays current beam position and
  the beam trajectory history.
- `disp_intensity.py`: A GUI program, displays the beam intensity history. This
  is for troubleshooting laser power fluctuations.
- `interactive_prompt.py`: Provide an interactive prompt, where the user can
  type in commands and those commands are relayed to the microcontroller. This
  is the primary way of manually controlling the microcontroller.
- `move_motor.py`: Move the motor back and forth and collecting the PSD
  readings at the same time. This is for acquiring the calibration data.
- `utils.py`: Utilities functions.
