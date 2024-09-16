# hasak cw keyer

Hasak is firmware for the CWKeyerShield described in
https://github.com/softerhardware/CWKeyer.  

With appropriate wiring, it also works on bare Teensy 
microprocessors, Teensy microprocessors with the Teensy
Audio Adapter, and Teensy microprocessors on hardware
specifically designed for hasak.

Hasak is designed to combine the Teensy hardware, the Teensy Audio
library, and the Musical Instrument Device Interface (MIDI) to make 
a highly configurable cw keyer that allows sample accurate timing.

## building
Obtain a Teensy 4.0 or 4.1 from https://www.pjrc.com/store/ or one
of the suppliers listed there.  (Teensy 3.x will work, but they've
been discontinued as products, so don't try to buy one.)

Install the Teensyduino development environment according to the 
instructions at https://www.pjrc.com/teensy/td_download.html

If you are running on Linux, do not skip the instructions for
Linux udev rules - Proper setup for USB devices on Linux systems.

Download the source for the project from https://github.com/recri/hasak

Open the project with the arduino-ide.  In the tools menu you need to 
select the Board, the Port, and the USB Type.

In the Board submenu, pick your Teensy version from the Teensy submenu.

In the Port submenu, select your Teensy's port.  The list of possible ports
will appear once you've connected your Teensy with a USB cable.

In the USB Type submenu, select "Serial+MIDI+Audio".

## wiring

pin 0 to right paddle in
pin 1 to left paddle in
pin 2 to straight key in
pin 3 to external PTT in
pin 4 to key out
pin 5 to PTT out
pin 10 to audio out left channel (Teensy 4.X)
pin 12 to audio out right channel (Teensy 4.X)



