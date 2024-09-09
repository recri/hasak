/* Teensy support for wm8960 audio codec
 * Copyright (c) 2021, Steve Haynal, steve@softerhardware.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Arduino.h>
#include "control_wm8960.h"

// See https://blog.titanwolf.in/a?ID=00500-80a77412-7973-49b3-b2e9-bc1beb847257


#define WM8960_I2C_ADDR 0x1A

// mask set bits are the bits in val which should be written
bool AudioControlWM8960::write(uint16_t reg, uint16_t val, uint16_t mask, bool force=false)
{

    uint16_t newval;

    newval = (regmap[reg] & ~mask) | (val & mask);

    if ((newval != regmap[reg]) || force) {
        regmap[reg] = newval;

        Wire.beginTransmission(WM8960_I2C_ADDR);

        Wire.write((reg << 1) | ((newval >> 8) & 1));
        Wire.write(newval & 0xFF);

        if (Wire.endTransmission() == 0) return true;
        return false;
    }
    return true;
}



bool AudioControlWM8960::disable(void)
{
    // Reset
    return write(0x0f, 0, 0b01, true);
}


bool AudioControlWM8960::enable(void)
{
    //Wire.begin();
    //delay(5);

    // Reset
    if (!write(0x0f, 0, 0b01, true)) {
        return false; // no WM8960 chip responding
    }

    delay(5);

    // Enable VMID and VREF
    write(0x19, 0b111000000, 0b111000000);

    // Enable DAC, Headphones and Speakers
    write(0x1a, 0b111111000, 0b111111000);

    // Enable mixer
    write(0x2f, 0b000001100, 0b000001100);

    // Setup clocking
    write(0x04, 0b000000000, 0b111111111);


    // Unmute DAC
    write(0x05, 0b000000000, 0b000001000);

    // 16-bit data and i2s interface
    write(0x07, 0b000000010, 0b000001111); // I2S, 16 bit, MCLK slave

    // Mute headphone and speakers, but enable zero crossing changes only
    write(0x02, 0b010000000, 0b011111111);
    write(0x03, 0b110000000, 0b111111111);
    // Speakers
    write(0x28, 0b010000000, 0b011111111);
    write(0x29, 0b110000000, 0b111111111);

    // Set DAC Volume to max 0dB, full range of DAC
    write(0x0a, 0b011111111, 0b011111111);
    write(0x0b, 0b111111111, 0b111111111);

    // Connect Left DAC to left output mixer
    write(0x22, 0b100000000, 0b100000000);

    // Connect Right DAC to right output mixer
    write(0x25, 0b100000000, 0b100000000);


    // Enable headphone detect to disable speaker
    // Enable HPSWEN and set HPSWPOL
    write(0x18, 0b001000000, 0b001100000);
    // Use JD02 as jack detect input
    write(0x30, 0b000001000, 0b000001100);
    // Enable slow clock for jack detect
    write(0x17, 0b000000001, 0b000000011);


    // Enable speaker outputs
    write(0x31, 0b011000000, 0b011000000);


    //// Configure input
    //// Enable AINL/AINR, and ADCL/ADCR
    write(0x19, 0b000111100, 0b000111100);

    // Unmute and set zero crossing input level change
    write(0x00, 0b101000000, 0b111000000);
    write(0x01, 0b101000000, 0b111000000);

    delay(100); // how long to power up?

    return true;
}

bool AudioControlWM8960::volume(float n) {
    headphoneVolume(n,n);
    return speakerVolume(n,n);
}

bool AudioControlWM8960::volume(float l, float r) {
    headphoneVolume(l,r);
    return speakerVolume(l,r);
}

bool AudioControlWM8960::headphoneVolume(float l, float r)
{
    uint16_t i;
    i = 47 + (uint16_t) (80.0*l+0.5);
    // Left headphone
    write(0x02, (i & 0b01111111), 0b001111111, true);

    i = 47 + (uint16_t) (80.0*r+0.5);
    // Right headphone
    return write(0x03, 0b100000000 | (i & 0b01111111), 0b101111111, true);
}

bool AudioControlWM8960::headphonePower(uint8_t p)
{
    uint16_t mask;
    uint16_t value;

    value = (p & 0b11) << 5;
    mask = 0b011 << 5;

    // LOUT1 Enable
    return write(0x1a, value, mask);
}

bool AudioControlWM8960::speakerVolume(float l, float r)
{
    uint16_t i;
    i = 47 + (uint16_t) (80.0*l+0.5);
    // Left speaker
    write(0x28, (i & 0b01111111), 0b001111111, true);

    i = 47 + (uint16_t) (80.0*r+0.5);
    // Right speaker
    return write(0x29, 0b100000000 | (i & 0b01111111), 0b101111111, true);
}

bool AudioControlWM8960::speakerPower(uint8_t p)
{
    uint16_t mask;
    uint16_t value;

    value = (p & 0b11) << 3;
    mask = 0b011 << 3;

    // SPK_PN Output Enable
    write(0x1a, value, mask);

    // SPK_OP_EN
    // Bits are swapped
    value = (p & 0b01) ? 0b010000000 : 0b0;
    value = (p & 0b10) ? (value | 0b001000000) : value;
    mask = 0b011 << 6;
    return write(0x31, value, mask);
}


// Write 1 to disable, 0 to enable, default is enabled
bool AudioControlWM8960::disableADCHPF(uint8_t v)
{
    return write(0x05, v, 0b01);
}


// Write 1 to enable
bool AudioControlWM8960::enableMicBias(uint8_t v)
{
    return write(0x19,v << 1,0b000000010);
}

// Write 1 to enable
bool AudioControlWM8960::enableALC(uint16_t v)
{
    // FIXME set LINVOL and RINVOL to same value if both are enabled
    return write(0x11, v << 7, 0b110000000);
}


bool AudioControlWM8960::micPower(uint8_t p)
{
    uint16_t mask;
    uint16_t value;


    // Select microphone inputs with +29dB boost
    if (p & 0b010) {
        // enable left
        write(0x20,0b100111000,0b111111000);
    } else {
        // disable left
        write(0x20,0b000000000,0b111111000);
    }

    if (p & 0b001) {
        // enable right (MEMS)
        write(0x21,0b100111000,0b111111000);
    } else {
        // disable left
        write(0x21,0b000000000,0b111111000);
    }

    // enable microphone inputs
    value = p  << 4;
    mask = 0b011 << 4;

    return write(0x2f, value, mask);
}

bool AudioControlWM8960::lineinPower(uint8_t p)
{
    uint16_t mask;
    uint16_t value;


    // Select line2
    if (p & 0b010) {
        // enable left
        write(0x20,0b001000000,0b111111000);
    } else {
        // disable left
        write(0x20,0b000000000,0b111111000);
    }

    if (p & 0b001) {
        // enable right (MEMS)
        write(0x21,0b001000000,0b111111000);
    } else {
        // disable left
        write(0x21,0b000000000,0b111111000);
    }

    // disable microphone inputs
    value = p << 4;
    mask = 0b011 << 4;
    return write(0x2f, value, mask);
}



bool AudioControlWM8960::inputLevel(float n)
{
    return inputLevel(n,n);

}

bool AudioControlWM8960::inputLevel(float l, float r)
{
    uint16_t i;
    i = (uint16_t) (63.0*l+0.5);
    // Left input
    write(0x0, i, 0b000111111, true);

    i = (uint16_t) (63.0*r+0.5);
    // Right input (MEMS)
    return write(0x1, 0b100000000 | i, 0b100111111, true);
}

bool AudioControlWM8960::inputSelect(int n)
{
    if (n) {
        return lineinPower(0b011);
    } else {
        return micPower(0b011);
    }
}
