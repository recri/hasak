#ifndef linkage_h_
#define linkage_h_

int get_active_vox(void);

extern int16_t kyr_nrpn[];

/* fetch a nrpn */
static inline int get_nrpn(int nrpn) { 
  return nrpn >= 0 && nrpn <= KYRP_LAST ? kyr_nrpn[nrpn] : -1;
}
/* set a nrpn */
static inline void set_nrpn(int nrpn, int value) {
  if (nrpn >= 0 && nrpn <= KYRP_LAST)
    kyr_nrpn[nrpn] = value;
}
/* fetch a vox specialized nrpn, not implemented */
static int get_vox_nrpn(int vox, int nrpn) {
  return get_nrpn(nrpn);
}
/* set a vox specialized nrpn, not implemented, fix.me */
static int set_vox_nrpn(int vox, int nrpn, int value) {
  set_nrpn(nrpn, value);
}

/***************************************************************
** Parameter fetching
** keyers, oscillators, ramps, and ptt fetch parameters as required
** which allows them to vary as you twiddle the ctrlr dials
***************************************************************/
/* keyer timing */
static inline int get_vox_dit(int vox) { return get_vox_nrpn(vox, KYRP_PER_DIT); }
static inline int get_vox_dah(int vox) { return get_vox_nrpn(vox, KYRP_PER_DAH); }
static inline int get_vox_ies(int vox) { return get_vox_nrpn(vox, KYRP_PER_IES); }
static inline int get_vox_ils(int vox) { return get_vox_nrpn(vox, KYRP_PER_ILS); }
static inline int get_vox_iws(int vox) { return get_vox_nrpn(vox, KYRP_PER_IWS); }

static inline int set_vox_dit(int vox, int value) { return set_vox_nrpn(vox, KYRP_PER_DIT, value); }
static inline int set_vox_dah(int vox, int value) { return set_vox_nrpn(vox, KYRP_PER_DAH, value); }
static inline int set_vox_ies(int vox, int value) { return set_vox_nrpn(vox, KYRP_PER_IES, value); }
static inline int set_vox_ils(int vox, int value) { return set_vox_nrpn(vox, KYRP_PER_ILS, value); }
static inline int set_vox_iws(int vox, int value) { return set_vox_nrpn(vox, KYRP_PER_IWS, value); }

/* keyed tone parameters */
static inline int get_vox_speed(int vox) { return get_vox_nrpn(vox, KYRP_SPEED); }
static inline int get_vox_weight(int vox) { return get_vox_nrpn(vox, KYRP_WEIGHT); }
static inline int get_vox_ratio(int vox) { return get_vox_nrpn(vox, KYRP_RATIO); }
static inline int get_vox_comp(int vox) { return get_vox_nrpn(vox, KYRP_COMP); }
static inline int get_vox_farns(int vox) { return get_vox_nrpn(vox, KYRP_FARNS); }

static inline int get_vox_tone(int vox) { return get_vox_nrpn(vox, KYRP_TONE); }
static inline int get_vox_iq_enable(int vox) { return get_vox_nrpn(vox, KYRP_IQ_ENABLE); } /* not really a vox property */
static inline int get_vox_iq_adjust(int vox) { return get_vox_nrpn(vox, KYRP_IQ_ADJUST); } /* not really a vox property */
static inline int get_vox_rise_time(int vox) { return get_vox_nrpn(vox, KYRP_RISE_TIME); }
static inline int get_vox_rise_ramp(int vox) { return get_vox_nrpn(vox, KYRP_RISE_RAMP); }
static inline int get_vox_fall_time(int vox) { return get_vox_nrpn(vox, KYRP_FALL_TIME); }
static inline int get_vox_fall_ramp(int vox) { return get_vox_nrpn(vox, KYRP_FALL_RAMP); }

/* ptt parameters */
static inline int get_vox_ptt_head(int vox) { return get_vox_nrpn(vox, KYRP_HEAD_TIME); }
static inline int get_vox_ptt_tail(int vox) { return get_vox_nrpn(vox, KYRP_TAIL_TIME); }

#endif
