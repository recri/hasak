# Ham and Swiss Army Knife (hasak) keyer
## Firmware for the Softerhardware/CWKeyerShield
The goal is to allow the operator to hear keyed code with low
latency while listening to and keying a possibly remote 
transceiver. 

The current release is built for the CWKeyer prototype.
The default setup mixes sidetone and received audio from
USB audio and sends that to the codec output.
   
The `cwkeyer-js` project provides a web app for controlling hasak
and other keyers and MIDI devices which identify themselves
sufficiently.
   
See [cwkeyer-js](https://github.com/recri/cwkeyer-js) for more
information about that.

## Other hardware
The `hasak.TEENSY40@600.hex` image should run on a bare Teensy4
and a Teensy4+AudioAdapter.  The code should also compile for a
bare Teensy3 and a Teensy3+AudioAdapter. But I need to rebuild the
variants to test that those still work.

## Hasak details
Hasak consists of a general purpose MIDI controller interface
and a code that implements a morse code keyer using that
interface.

### Midi interface
The midi interface is provided by `midi.h`.  This manages incoming 
and outgoing midi traffic, maintains arrays of current note (note), 
control change (ctrl), and nonregistered parameter (nrpn)
values. It also supports listeners that are fired when any of the
values are written (or almost written). 

The interface is written as a C++ template class which accepts
template parameters for the number of notes, control changes, and
non-registered parameters (NRPNs) to support. The template class
constructor takes a Teensy `usb_midi_class` reference and a channel
number.

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
provides the storage for a list element.  The latency of calling a
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

