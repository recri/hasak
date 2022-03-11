#!/usr/bin/tclsh

#
# can I pass a negative frequency into the oscillator phase increment
# and get things to work out?
#
# do I need to adjust the IQ phase accumulators or will they
#

# xnrpn_set(NRPN_XTONE, signed_value(nrpn_get(NRPN_TONE))*1000); // Hz to Hz/1000

#    phase_I = +45.0 * (4294967296.0 / 360.0);
#    phase_Q = (360-45.0) * (4294967296.0 / 360.0);
#    if (signed_xvalue(get_xnrpn(NRPN_XTONE)) < 0) {
#      phase_I = (360-45.0) * (4294967296.0 / 360.0);
#      phase_Q = +45.0 * (4294967296.0 / 360.0); 
#    }

#  uint32_t phase_increment(void) {
#    return signed_xvalue(get_xnrpn(NRPN_XTONE)) * 0.001f * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT); // convert from thousandths of hertz to hertz
#  }

proc phinc {freq} { expr {$freq * 4294967296 / 48000} }
proc phincf {freq} { expr {int($freq * 4294967296.0 / 48000.0)} }

for {set f 500} {$f < 1500} {incr f 50} {
    foreach t [list $f [expr {-$f}]] {
	set phi [phinc $t]
	set phif [phincf $t]
	set fphi [expr $phi/4294967296.0]
	set fphif [expr $phif/4294967296.0]
	puts [format "freq %d %d %.5f %d %.5f" $f $phi $fphi $phif $fphif]
    }
}
