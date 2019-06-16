# Kaleidoscope-Simulator-Recorder

[Kaleidoscope-Simulator-Recorder](https://github.com/CapeLeidokos/Kaleidoscope-Simulator-Recorder.git) is an auxiliary plugin that can be used to generate I/O protocols 
of the keyboard. See the description of the library [Aglais](https://github.com/CapeLeidokos/Aglais.git)
that this plugin depends on for more information about the generated
data format.

## Collecting I/O protocols

If the plugin is active in the firmware, the keyboard signals its readyness
for recording by blinking three times red and then one time green.
This gives you enough time to pipe the data that is received on the host
system to a file. This data is arriving through the serial interface.

```
cat /dev/ttyACM0 > io_protocol.agl
```

The generated file is an Aglais document. It can be compressed using `aglais_convert`
and then directly added as quoted C++-string to a test that 
can be used with [Kaleidoscope-Simulator](https://github.com/CapeLeidokos/Kaleidoscope-Simulator.git).

```
aglais_convert io_protocol.agl io_protocol.compressed.agl
```
