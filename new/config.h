/* an incomplete segment of config.h dealing with the new common nrpns */
#if defined(NRPN_REVISION)
#define KYRP_NNRPN              (KYRP_FIRST+3) /* {type inf title {number of NRPNs} property keyerNNrpn} */
#define NRPN_QUERY              (KYRP_FIRST+4) /* {type cmd title {take value as NRPN and return its value, or no response} property keyerQuery} */
#define NRPN_UNSET              (KYRP_FIRST+5) /* {type cmd title {take the value as a nrpn number and make that nrpn unset} property keyerUnset} */
#define NRPN_CHANNEL		(KYRP_FIRST+6) /* {type par title {the MIDI channel} property keyerChannel} */
#define NRPN_ID_JSON		(KYRP_FIRST+7) /* {type cmd title {send the JSON string which describes the keyer} property keyerJSON} */
#define NRPN_STRING_BEGIN	(KYRP_FIRST+8) /* {type cmd title {begin a string transfer} property keyerStringBegin} */
#define NRPN_STRING_END		(KYRP_FIRST+9), /* {type cmd title {finish a string transfer} property keyerStringEnd} */
#define NRPN_STRING_BYTE	(KYRP_FIRST+10) /* {type cmd title {transfer a byte for a string} propertyStringByte} */

#define KYRP_WM8960		(KYRP_FIRST+11) /* {type rel title {start of CWKeyerShield WM8960 commands}} */
    MIDI_NRPN_WM8960_ENABLE            = 11,
    MIDI_NRPN_WM8960_INPUT_LEVEL       = 12,
    MIDI_NRPN_WM8960_INPUT_SELECT      = 13,
    MIDI_NRPN_WM8960_VOLUME            = 14,
    MIDI_NRPN_WM8960_HEADPHONE_VOLUME  = 15,
    MIDI_NRPN_WM8960_HEADPHONE_POWER   = 16,
    MIDI_NRPN_WM8960_SPEAKER_VOLUME    = 17,
    MIDI_NRPN_WM8960_SPEAKER_POWER     = 18,
    MIDI_NRPN_WM8960_DISABLE_ADCHPF    = 19,
    MIDI_NRPN_WM8960_ENABLE_MICBIAS    = 20,
    MIDI_NRPN_WM8960_ENABLE_ALC        = 21,
    MIDI_NRPN_WM8960_MIC_POWER         = 22,
    MIDI_NRPN_WM8960_LINEIN_POWER      = 23,
    MIDI_NRPN_WM8960_RAW_MASK          = 24,
    MIDI_NRPN_WM8960_RAW_DATA          = 25,
    MIDI_NRPN_WM8960_RAW_WRITE         = 26,
    MIDI_NRPN_KEYDOWN_NOTE             = 27,
    MIDI_NRPN_PTT_NOTE                 = 28
#define KYRP_CODEC		(KYRP_WM8960+18) /* {type rel title {base of codec nrpns}} */
#endif
