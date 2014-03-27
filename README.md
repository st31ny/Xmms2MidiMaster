Xmms2MidiMaster
===============

XMMS2-Client emitting MIDI timecode to synchronize arbitrary MIDI-capable devices

## Required packages

This software uses Makefiles and `g++`, although it should be compilable with different standard C++-Compilers as well.

In order to compile the following packages/libraries are required:

- `libportmidi-dev`
- `xmmsclient++-dev` (available from the [XMMS2-Git-Site][xmms2-git])
    - `libboost-dev`
    - `libsqlite-dev`
    - `libpython-dev`
- `libboost-program-options-dev`

A `Makefile` is provided to build the program. Available targets are:

- `all`: Compile the program
- `doc`: Generate the source code documentation
- `clean`: Remove all generated files (object files, binaries, generated documentation)

Furthermore `doxygen` is required to generate the source code documentation.

[xmms2-git]: http://git.xmms.se/

