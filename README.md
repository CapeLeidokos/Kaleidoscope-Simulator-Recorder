# Kaleidoscope-Simulator-Recorder

A plugin that records typing sessions.

## Purpose

When operating a Kaleidoscope driven keyboard, key actions (keys being pressed and released) cause
the firmware to emit different type of USB HID reports that are send to the host system. Those reports inform the host system
about input through the USB devices (keyboard, boot-keyboard, mouse and absolute mouse) as which the keyboard registeres with the host system.

For simulation based integration testing, e.g. with [Kaleidoscope-Simulator](https://github.com/CapeLeidokos/Kaleidoscope-Simulator.git],
the exact timing of key actions and generated HID reports is of great importance. The simulator is expected to produce the same series of HID reports as the physical keyboard when confronted with key action. The I/O data of a keyboard session can thus be used to define tests that are executed by the simulator. Failing tests indicates that the I/O behavior of the firmware has changed compared to the firmware that was used to record the test data.

Kaleidoscope-Simulator-Recorder collects timing information and sends them via the serial interface to the host system in [Aglais](https://github.com/CapeLeidokos/Aglais.git) data format.

## Application

### Installing Aglais

Kaleidoscope-Simulator-Recorder requires the Arduino library [Aglais](https://github.com/CapeLeidokos/Aglais.git)
to be installed.

### Preparing the sketch

Include the header `Kaleidoscope-Simulator-Recorder.h` in your sketch and add the plugin to the list of plugins.
Please make sure that Kaleidoscope-Simulator-Recorder is the first in the list.

```cpp
...
#include "Kaleidoscope-Simulator-Recorder.h"
...

KALEIDOSCOPE_INIT_PLUGINS(
   SimulatorRecorder,
   ...
)
```

### Operating the keyboard

Once Kaleidoscope-Simulator-Recorder is part of the running firmware, the device's LEDs will flash three times red and one time green every time it is plugged into the host. This is to inform the user about recording and data transfer to start soon. Data transfer starts right after the intro sequence. The sequence provides enough time to redirect serial output to a file before the recording starts.

### Collecting data

The stream of data send from the keyboard is best redirected to a file, e.g. on a GNU/Linux system typically as

```
cat /dev/ttyACM0 > io_protocol.agl
```

The generated file is an Aglais document. It can be compressed using `aglais_convert`
and then directly added as quoted C++-string to a test that 
can be used with [Kaleidoscope-Simulator](https://github.com/CapeLeidokos/Kaleidoscope-Simulator.git).

```
aglais_convert io_protocol.agl io_protocol.compressed.agl
```
