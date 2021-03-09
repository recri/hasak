#ifndef my_control_sgtl5000_h_
#define my_control_sgtl5000_h_

#include <control_sgtl5000.h>

#define CHIP_ANA_ADC_CTRL		0x0020
// 8	ADC_VOL_M6DB	ADC Volume Range Reduction
//				This bit shifts both right and left analog ADC volume
//				range down by 6.0 dB.
//				0x0 = No change in ADC range
//				0x1 = ADC range reduced by 6.0 dB
// 7:4	ADC_VOL_RIGHT	ADC Right Channel Volume
//				Right channel analog ADC volume control in 1.5 dB steps.
//				0x0 = 0 dB
//				0x1 = +1.5 dB
//				...
//				0xF = +22.5 dB
//				This range is -6.0 dB to +16.5 dB if ADC_VOL_M6DB is set to 1.
// 3:0	ADC_VOL_LEFT	ADC Left Channel Volume
//				(same scale as ADC_VOL_RIGHT)

#define CHIP_ANA_CTRL			0x0024
// 8	MUTE_LO		LINEOUT Mute, 0 = Unmute, 1 = Mute  (default 1)
// 6	SELECT_HP	Select the headphone input, 0 = DAC, 1 = LINEIN
// 5	EN_ZCD_HP	Enable the headphone zero cross detector (ZCD)
//				0x0 = HP ZCD disabled
//				0x1 = HP ZCD enabled
// 4	MUTE_HP		Mute the headphone outputs, 0 = Unmute, 1 = Mute (default)
// 2	SELECT_ADC	Select the ADC input, 0 = Microphone, 1 = LINEIN
// 1	EN_ZCD_ADC	Enable the ADC analog zero cross detector (ZCD)
//				0x0 = ADC ZCD disabled
//				0x1 = ADC ZCD enabled
// 0	MUTE_ADC	Mute the ADC analog volume, 0 = Unmute, 1 = Mute (default)

#define CHIP_MIC_CTRL			0x002A // microphone gain & internal microphone bias
// 9:8	BIAS_RESISTOR	MIC Bias Output Impedance Adjustment
//				Controls an adjustable output impedance for the microphone bias.
//				If this is set to zero the micbias block is powered off and
//				the output is highZ.
//				0x0 = Powered off
//				0x1 = 2.0 kohm
//				0x2 = 4.0 kohm
//				0x3 = 8.0 kohm
// 6:4	BIAS_VOLT	MIC Bias Voltage Adjustment
//				Controls an adjustable bias voltage for the microphone bias
//				amp in 250 mV steps. This bias voltage setting should be no
//				more than VDDA-200 mV for adequate power supply rejection.
//				0x0 = 1.25 V
//				...
//				0x7 = 3.00 V
// 1:0	GAIN		MIC Amplifier Gain
//				Sets the microphone amplifier gain. At 0 dB setting the THD
//				can be slightly higher than other paths- typically around
//				~65 dB. At other gain settings the THD are better.
//				0x0 = 0 dB
//				0x1 = +20 dB
//				0x2 = +30 dB
//				0x3 = +40 dB

class MyAudioControlSGTL5000 : public AudioControlSGTL5000
{
public:
 MyAudioControlSGTL5000(void) : AudioControlSGTL5000() { }
  // override inputSelect to not step on gain controls
  // write(CHIP_MIC_CTRL, 0x0173) overwites the mic bias and impedance
  bool inputSelect(int n) {
    if (n == AUDIO_INPUT_LINEIN) {
      return // write(CHIP_ANA_ADC_CTRL, 0x055) && // +7.5dB gain (1.3Vp-p full scale)
	write(CHIP_ANA_CTRL, ana_ctrl | (1<<2)); // enable linein
    } else if (n == AUDIO_INPUT_MIC) {
      return // write(CHIP_MIC_CTRL, 0x0173) && // mic preamp gain = +40dB 
	// write(CHIP_ANA_ADC_CTRL, 0x088) && // input gain +12dB (is this enough?)
	write(CHIP_ANA_CTRL, ana_ctrl & ~(1<<2)); // enable mic
    } else {
      return false;
    }
  }

  // override micGain to not step on bias and impedance controls
  bool micGain(unsigned int dB) {
    unsigned int preamp_gain, input_gain;

    if (dB >= 40) {
      preamp_gain = 3;
      dB -= 40;
    } else if (dB >= 30) {
      preamp_gain = 2;
      dB -= 30;
    } else if (dB >= 20) {
      preamp_gain = 1;
      dB -= 20;
    } else {
      preamp_gain = 0;
    }
    input_gain = (dB * 2) / 3;
    if (input_gain > 15) input_gain = 15;

    return write(CHIP_MIC_CTRL, (read(CHIP_MIC_CTRL) & ~0xF) | preamp_gain) && 
      write(CHIP_ANA_ADC_CTRL, (input_gain << 4) | input_gain);
  }
  // override micGain to not step on bias and impedance controls
  bool micPreampGain(unsigned int gain) {
    return write(CHIP_MIC_CTRL, (read(CHIP_MIC_CTRL) & ~0x3) | gain);
  }
  bool lineInLevel(uint8_t left, uint8_t right) {
    if (left > 15) left = 15;
    if (right > 15) right = 15;
    return write(CHIP_ANA_ADC_CTRL, (left << 4) | right);
  }
 // add micBias control
  bool micBias(unsigned int bias) {
    return write(CHIP_MIC_CTRL, (read(CHIP_MIC_CTRL) & ~0x70) | ((bias&7)<<4));
  }
  // add micImpedance control
  bool micImpedance(unsigned int impedance) {
    return write(CHIP_MIC_CTRL, (read(CHIP_MIC_CTRL) & ~0x300) | ((impedance&3)<<8));
  }
};
#endif
