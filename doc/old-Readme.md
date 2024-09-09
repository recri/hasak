# Ham and Swiss Army Knife (hasak) keyer
## Firmware for the Softerhardware/CWKeyerShield
The goal is to allow the operator to hear keyed code with low
latency while listening to and keying a possibly remote 
transceiver.

The current release is built for the CWKeyer prototype with a Teensy
4.0.  The default setup mixes sidetone and received audio from USB 
audio and sends that to the codec and MQS outputs.
   
The [`cwkeyer-js`](https://github.com/recri/cwkeyer-js) project
provides a web app for controlling hasak and other keyers and MIDI
devices which identify themselves sufficiently.  Try it at
[cwkeyer.elf.org](https://cwkeyer.elf.org).
   

## Other hardware
The `hasak/hasak.TEENSY40@600.hex` image in github should also run on a
bare Teensy4.0 or a Teensy4.0+AudioAdapter.  The code should also compile
for Teensy3.x or Teensy4.1 either bare or with an AudioAdapter, and
the Teensy 4.1 should work in the CWKeyer prototype. I need to build
the variants to test that those still work.

Operating as a keyer with headphones connected to the Teensy 4 medium
quality sound channels (with a simple RC lowpass or a transformer to
clean up the buzz) is highly recommended, so you can hear what the
codec adds to the experience.  It also lets you free up the codec so
it can do linein/lineout stereo.

## Hasak details
Hasak consists of a general purpose MIDI controller interface
and a code that implements a morse code keyer using that
interface.

The hasak source directory contains `hasak.ino`, the Arduino sketch
for the project, and an assortment of C header files that `hasak.ino`
includes.  The `src` subdirectory contains C++ source headers, and the
`src/Audio` directory contains the C++ headers and implementaion files
which define components for the Teensy Audio library.

### Why so many files?

The MIDI and keyer functions are broken into files with well defined
jobs, so they can each focus on doing their job.  And the coder can
often get all the code that does a job on one page.

### Why so many `static` declarations?

The `static` storage class identifies objects which cannot be seen
outside their compilation unit, which is `hasak.ino`.  So the compiler
knows all the possible uses for a `static` data and functions.  It is
free to expand `static` functions `inline` where possible.  And no
actual definition ever needs to be generated if no one takes the
address of a `static` function.

### Midi interface
The midi interface is provided by `midi.h` with the assistance of
`src/midi.h`.  This manages incoming and outgoing midi traffic,
maintains arrays of current note (`NOTE`), control change (`CTRL`),
and nonregistered parameter (`NRPN`) values. It implements a firewall
that allows any subset of incoming messages to be enabled or ignored,
any subset of outgoing messages to be enabled for sending, any subset
of input messages to be echoed back, and any subset of messages to be
marked read only.  It also supports listeners that are fired when any
of the `NOTE`, `CTRL`, or `NRPN` values are set (or almost set, if
read only).

The interface is written as a C++ template class which accepts
template parameters for `N_NOTE`, `N_CTRL`, and `N_NRPN`.  `NOTE`
ranges from `0` to `N_NOTE-1`, `CTRL` from `0` to `N_CTRL-1`, and
`NRPN` from `0` to `N_NRPN-1`. `NRPN` entry requires `N_CTRL > 99`,
but other than that `midi.h` supports any range you like. The template
class constructor takes a Teensy `usb_midi_class` reference and a
channel number. 

Each note, control change, and NRPN, collectively the state values,
can be independently enabled for input, output, echo, and writing.
State values - whether notes, controls, or NRPNs - that are not
input enabled are ignored on input.  State values that are not
output enabled never leave the device, though they can be
indirectly leaked.  If a state value is both input and output
enabled, then it can additionally be echo enabled, which means that
the message is echoed back to acknowledge receipt. If the state
value is read only, then input values never alter the controllers'
state, though they can still generate listener events.  If a state
value is input enabled, output enabled, echo enabled, and read
only, then the echo will contain the controller's value rather than
the value in the input state value.

So the interface maintains an array of bytes for notes, an array of
bytes for control changes, an array of short ints for NRPNs, and an
array of bytes for flags.  The device side can read and write the
values and modify the flags. The host side can write the values
with MIDI where enabled.  The only action on received state values
which the interface takes by default is to trigger NRPN assembly
when it receives MIDI control change 38.
   
Most state values are read directly from the midi interface when
required rather than acted upon in the controller. That is, there
is no midi interface code checking for the frequency setting to be
changed in order to implement the frequency change.  The oscillator
control pulls the current frequency setting from the midi interface
when it is time to update the oscillator frequency.  And there is
no code looking at the sidetone key line to see whether the
oscillator in the audio graph needs to be enabled.  The sidetone
key line is mirrored directly from the midi interface array into
the audio graph in a sample rate interrupt.  Many of the important
actions in any audio application can be done this way.

State values which need immediate implementation are handled by
listeners. Each note, control, or NRPN can have listeners attached
to it.  When the interface sets the value of the note, control, or
NRPN, (or would have set the value if it weren't read only) it
invokes the listeners registered on the state value.  A listener is
a function which takes one argument - the note, control, or NRPN
number - and returns nothing.  Listeners have no overhead in the
interface other than a list head.  Each listener registered
provides the storage for a list element. which makes the macros that
register listeners a little fragile and peculiar.  The latency of calling a
listener is the latency of a function call.
   
### Keyer implementation

The keyer logic is implemented via the listener mechanism. Hardware
switch states are reflected into hardware switch notes, which
triggers listeners. Each keyer implementation listener reflects its
assigned hardware switch notes, either directly for a straight key
or through an iambic keyer, into a keyer implementation sidetone
note, which triggers listeners. There are different sidetone notes
for each possible key line, one for each straight key, one for each
iambic keyer, one for each text keyer, and one for each remote
keyer.  Sidetone arbitration listeners pick one of the active
sidetone notes, grant it the actual sidetone key, and reflect it
onto the actual keyer sidetone note, which triggers listeners. The
cwptt listener generates a `tx_ptt` note, delays and stretches the
actual sidetone key note, and reflects it to `tx_key` note, which
triggers listeners.  The listeners to `st_key`, `tx_key` and
`tx_ptt` can reflect the note to hardware pins, or reflect to the
host as a midi note, or both, or neither.  The `st_key` and `tx_key`
note listeners can also reflect the notes into the audio graph to
key the sideton or IQ oscillators.
   
## Source code organization
### `hasak.ino`
contains the top-level layout of the firmware. Most of the include
directives for files in `hasak` directory are to inline code into
`hasak.ino`.
### `config.h`
defines the pins, notes, controls, and nrpns the keyer uses, and
a few configuration options.
### `linkage.h`
contains definitions and declarations needed for separately compiled
c++ and c files.
### `src/`
This directory contains c++ code `src/*.h` are C++ class definitions
that are used directly in the `hasak/*.h` files, `src/Audio/*` are
definitions of custom audio library components.

