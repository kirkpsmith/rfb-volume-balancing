The code in this folder is meant to help maintain volume-balanced reservoirs during benchtop flow battery operation.

`timeavg.py` is a class for computing moving averages of real-time sensor data, used by `levelsensor.py`

`levelsensor.py` uses OpenCV to determine reservoir volumes from a camera feed. Aim a USB camera as close to perpendicular as possible to the liquid level height in the reservoirs. Run this script from the command line, follow the instructions, and it will begin logging reservoir level data to a file. You can also monitor electrolyte leakage/solvent evaporation in addition to imbalance, as it simply records the two individual levels.

`PID.py` reads the most recent logged data from `levelsensor.py` and computes control outputs accordingly, sending them over an instance of `nodeforwarder.js` (which manages the serial connection) in order to communicate with the microcontroller running `controller.cpp`.

`nodeforwarder.js` is my [fork](https://github.com/kirkpsmith/nodeforwarder) of Dan Steingart's nodeforwarder code. The main modification I have to the file in this repository to log the output of the microcontroller to record the actual pump speeds. `PID.py` probably should be written to log this data instead using the read functions of nodeforwarder, so that the nodeforwarder code can remain intact and not have to be included in this repository.

`controller.cpp` is the code designed to control KNF FF-12 diaphragm pumps using a Teensy 3.5 microcontroller. These pumps take a 0-5V input which is approximated by the analog output of the microcontroller. I've uploaded the code using both PlatformIDE and Arduino (with Teensyduino add-on). I also previously used this code to control Cole-Parmer peristaltic pumps, which take 0-10V input, by adding an op-amp circuit to double the voltage.

If you have issues getting this admittedly inadequately documented code to run, feel free to reach out and we will get it sorted. If you make your improvements also feel free to pull request. If you use this for academic work cite me pretty please.