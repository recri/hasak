#define KYRC_USE_RESPAR 1
//#define KYRC_USE_HOMEGR 1

#if defined(KYRC_USE_RESPAR)
#include <ResponsiveAnalogRead.h>

#define RARSleep true
#define RARSnap 0.001
#define RARActive 4.0
static ResponsiveAnalogRead input_adc0(KYR_ADC0, RARSleep, RARSnap); static short input_0;
static ResponsiveAnalogRead input_adc1(KYR_ADC1, RARSleep, RARSnap); static short input_1;
static ResponsiveAnalogRead input_adc2(KYR_ADC2, RARSleep, RARSnap); static short input_2;
static ResponsiveAnalogRead input_adc3(KYR_ADC3, RARSleep, RARSnap); static short input_3;
static ResponsiveAnalogRead input_adc4(KYR_ADC4, RARSleep, RARSnap); static short input_4;
static ResponsiveAnalogRead *input_adc[5] = { &input_adc0, &input_adc1, &input_adc2, &input_adc3, &input_adc4 };
#endif

static const uint8_t input_enable[5] = { 0b10000, 0b01000, 0b00100, 0b00010, 0b00001 };
static const int16_t input_nrpn[5] = {
  KYRP_ADC0_CONTROL, KYRP_ADC1_CONTROL, KYRP_ADC2_CONTROL, KYRP_ADC3_CONTROL, KYRP_ADC4_CONTROL
};
static const bool input_invert[5] = { false, true, true, true, true };
static int16_t input_value[5] = { 0, 0, 0, 0, 0 };

static void input_setup(void) {
#if defined(KYRC_USE_RESPAR)
  input_adc0.setActivityThreshold(RARActive);
  input_adc1.setActivityThreshold(RARActive);
  input_adc2.setActivityThreshold(RARActive);
  input_adc3.setActivityThreshold(RARActive);
  input_adc4.setActivityThreshold(RARActive);
#endif
}

// map 10 bit value into internal units and range
static int16_t input_cook_value(const int16_t nrpn2, const int16_t value, int16_t *cooked) {
  switch (nrpn2) {     /* decide how to map the value into the nrpn */
  case KYRP_TONE:      /* decihertz 200.0 to 1200.0 */
    *cooked = (2500+10*value) & KYRV_MASK; return 1;
  case KYRP_SPEED:		/* WPM 5 .. 69 */
  case KYRP_FARNS:
    *cooked = (5+(45*value/1024)) & KYRV_MASK; return 1;
  case KYRP_SPEED_FRAC:		/* 128th of a WPM */
    *cooked = (value>>2) & KYRV_MASK; return 1;
  case KYRP_BUTTON_0:		/* -8192 .. +8191 arbitrary units */
  case KYRP_BUTTON_1:
  case KYRP_BUTTON_2:
  case KYRP_BUTTON_3:
  case KYRP_BUTTON_4:
  case KYRP_IQ_ADJUST:
  case KYRP_IQ_BALANCE:
  case KYRP_ST_PAN:
    *cooked = ((value-512)*4) & KYRV_MASK; return 1;
  case KYRP_DEBOUNCE:		/* 0 .. 16383 samples */
  case KYRP_HEAD_TIME:
  case KYRP_TAIL_TIME:
  case KYRP_RISE_TIME:
  case KYRP_FALL_TIME:
  case KYRP_COMP:
    *cooked = (12*value) & KYRV_MASK; return 1;
  case KYRP_WEIGHT:		/* 25 .. 75 percent */
  case KYRP_RATIO:
    *cooked = (25+50*value/1024) & KYRV_MASK; return 1;
  case KYRP_VOLUME:		  /* all these and the *_MIX_* are dB/4 */
  case KYRP_LEVEL:		  /* -50*4 .. +6*4, -200 .. 24  */
  case KYRP_INPUT_LEVEL:
    *cooked = (-200+225*value/1024) & KYRV_MASK; return 1;
  default:
    if (nrpn2 >= KYRP_MIX_USB_L0 && nrpn2 <= KYRP_MIX_HDW_R3) {
      *cooked = (-200+225*value/1024) & KYRV_MASK; return 1;
    } else {
      Serial.printf("missing translation in input_update for nrpn %d\n", nrpn2);
    }
  }
  return 0;
}

# if defined(KYRC_USE_RESPAR)
static int input_update(const int16_t adc_number) {
  int16_t nrpn = input_nrpn[adc_number];
  bool enable = (get_nrpn(KYRP_ADC_ENABLE) & input_enable[adc_number]) != 0;
  ResponsiveAnalogRead *adc = input_adc[adc_number];
  int16_t *valuep = &input_value[adc_number];
  bool invert = input_invert[adc_number];
  int16_t nrpn2, raw_value, cooked_value;
  if ( ! enable) return 0; /* if this input is not enabled */
  nrpn2 = get_nrpn(nrpn);  /* fetch the nrpn controlled by this adc */
  if (nrpn2 == 0) return 0; /* if this input is not attached to anything */
  adc->update();		    /* update the reader */
  raw_value = invert ? 1023-adc->getValue() : adc->getValue(); /* get the updated value */
  if (raw_value == *valuep) return 0; /* if the raw value has not changed */
  *valuep = raw_value;		      /* save the new value */
  if ( ! input_cook_value(nrpn2, raw_value, &cooked_value)) return 0; /* if the cooker errors */
  if (cooked_value == get_nrpn(nrpn2)) return 0; /* if the cooked value hasn't changed */
  nrpn_set(nrpn2, cooked_value);		 /* set the new value */
  return 1;					 /* signal a change */
}
#endif

static void input_loop(void) {
  if (input_update(0)
      + input_update(1)
      + input_update(2)
      + input_update(3)
      + input_update(4) != 0)
    Serial.printf("inputs 0 1 2 3 4 =  %d %d %d %d %d\n", 
		  get_nrpn(get_nrpn(KYRP_ADC0_CONTROL)), 
		  signed_value(get_nrpn(get_nrpn(KYRP_ADC1_CONTROL))),
		  signed_value(get_nrpn(get_nrpn(KYRP_ADC2_CONTROL))),
		  get_nrpn(get_nrpn(KYRP_ADC3_CONTROL)),
		  get_nrpn(get_nrpn(KYRP_ADC4_CONTROL)));
}
