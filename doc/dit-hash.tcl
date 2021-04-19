#!/usr/bin/tclsh

package require Tk

# set up a pipeline from arecord, read into input buffers
set f [open {|arecord -D hw:CARD=hasak,DEV=0 -f S16_LE -c 2} rb]
fconfigure $f -blocking false -buffersize 4096 -translation binary
fileevent $f readable [list read-buffer $f]

set input -1
set inputs [dict create]
proc read-buffer {f} {
    dict set ::inputs [incr ::input] [read $f]
}

# process input buffers
set proc -1;			# input buffer pointer
set sample -12;			# sample counter

set idits [dict create];	# idit record
set izero 1;			# i is running zeros
set idit {};			# accumulating dit in i
set qdits [dict create];	# qdit record
set qzero 1;			# q is running zeros
set qdit {};			# accumulating dit in q

# accumulate average element length
set lengths 0
set nlengths 0

set alldits [dict create];	# accumulating records for all dits

# wave header as short ints little endian
set wave {}

# count the number of zeros at the end of a dit
proc zlength {s} {
    for {set nz 0} {$nz < [llength $s]} {incr nz} {
	if {[lindex $s end-$nz] != 0} {
	    break
	}
    }
    return $nz
}

# trim leading and trailing zeros
proc ztrim {s} {
    while {[lindex $s 0] == 0} { set s [lrange $s 1 end] }
    while {[lindex $s end] == 0} { set s [lrange $s 0 end-1] }
    return $s
}

# accumulate the strings of samples which form dits
proc process {sample iq val} {
    upvar \#0 ${iq}zero zero
    upvar \#0 ${iq}dit dit
    upvar \#0 ${iq}dits dits
    upvar \#0 ${iq}start start
    if {$zero} {
	if {$val != 0} {
	    set zero 0
	    set dit {}
	    lappend dit $val
	    set start $sample
	}
    } else {
	if {$val != 0} {
	    lappend dit $val
	} elseif {[zlength $dit] >= 32} {
	    set zero 1
	    set dit [ztrim $dit]
	    dict lappend dits $dit $start
	    incr ::lengths [llength $dit]
	    incr ::nlengths
	    # puts "[llength $dit] samples [llength [dict get $dits $dit]] instances"
	} else {
	    lappend dit $val
	}
    }
}

# find continuous ranges of tag in dict that are longer than min
proc findranges {dict offset min} {
    set start -1
    set end -1
    set ranges {}
    for {set i 0} {$i < [dict size $dict]} {incr i} {
	if {[dict exists $dict $i offs] && [lsearch -exact -integer [dict get $dict $i offs] $offset] >= 0} {
	    if {$start == -1} { set start $i }
	    set end $i
	} else {
	    if {$start != -1} {
		if {$end-$start+1 >= $min} {
		    lappend ranges $start $end
		}
		set start -1
		set end -1
	    }
	}
    }
    if {$start != -1} {
	if {$end-$start+1 >= $min} {
	    lappend ranges $start $end
	}
	set start -1
	set end -1
    }
    return $ranges
}

proc findpatch {ch el samples} {
    upvar \#0 map$ch$el map
    upvar \#0 correct$ch$el correct
    # index the samples against the correct samples 
    set ns [llength $samples];	# length of candidate sequence to match
    set nc [llength $correct];	# length of correct sequence
    set index [dict create];	# working map for sample index to correct index
    # scan once to build index and classify
    set i 0
    set offsets [dict create]
    foreach s $samples {
	set m {}
	if {[dict exists $map $s]} { set m [dict get $map $s] }
	set dict [dict create map $m offs {}]
	foreach x $m {
	    set o [expr {$x-$i}]
	    if {abs($o) <= 128} {
		dict incr offsets $o
		dict lappend dict offs $o
	    }
	}
	dict set index $i $dict
	incr i
    }
    if {0} {
	foreach o [lsort -integer -increasing [dict keys $offsets]] {
	    if {[dict get $offsets $o] > 4} {
		puts -nonewline " $o [dict get $offsets $o]"
	    }
	}
	puts {}
    }
    # accumulate sequences of 3 or more of any offset
    set ranges [dict create]
    dict for {o n} $offsets {
	if {$n > 4} {
	    set r [findranges $index $o 4]
	    if {$r ne {}} {
		dict set ranges $o $r
	    }
	}
    }
    return $ranges
}

proc format-wave {} {
    if {[llength $::wave] == 22} {
	binary scan [binary format s* $::wave] A4iA4A4issiissA4i ChunkID ChunkSize Format Subchunk1ID Subchunk1Size \
	    AudioFormat NumChannels SampleRate ByteRate BlockAlign BitsPerSample Subchunk2ID Subchunk2Size
	return [list $ChunkID $ChunkSize $Format $Subchunk1ID $Subchunk1Size \
	     $AudioFormat $NumChannels $SampleRate $ByteRate $BlockAlign $BitsPerSample $Subchunk2ID $Subchunk2Size]
    }
    puts "wrong length for wave header [llength $::wave] != 22"
    return $::wave
}

proc process-inputs {} {
    ## process new inputs
    ## puts "$::proc $::input"
    set ndone $::nlengths
    while {$::proc < $::input} {
	binary scan [dict get $::inputs [incr ::proc]] {s*} samples
	dict set ::inputs $::proc {}
	# puts [llength $samples]
	foreach {i q} $samples {
	    incr ::sample
	    if {$::sample < 0} {
		lappend ::wave $i $q
	    } else {
		process $::sample i $i
		process $::sample q $q
	    }
	}
    }

    ## process all completed elements into text after 
    if {$::nlengths > 16 && $::nlengths > $ndone} { after idle process-elements }
    after 500 [list process-inputs]
}

proc process-elements {} {
    set boundary [expr {$::lengths/double($::nlengths)}]
    set dits {}
    foreach ch {i q} dict [list $::idits $::qdits] {
	dict for {dit pos} $dict {
	    set n [llength $pos]
	    set l [llength $dit]
	    # if the element has already been analyzed
	    # we only update the position list and frequency
	    # and skip repeating the sequence analysis
	    if {[dict exists $::alldits $dit]} {
		dict set ::alldits $dit pos $pos
		dict set ::alldits $dit freq $n
		continue
	    }
	    if {$l < $boundary} {
		set el {dit}
	    } else {
		set el {dah}
	    }
	    lappend dits [list $ch $el $n $pos $l $dit ]
	}
    }

    # sort by frequency, correct wave forms are most frequent
    # if they're present in this tranche
    # note that we change the names of n and l from previous loop
    foreach dit [lsort -index 2 -integer -decreasing $dits] {
	# break out the fields again
	foreach {ch el freq pos n samples} $dit break
	upvar \#0 map$ch$el map
	upvar \#0 correct$ch$el correct
	if { ! [info exists correct]} {
	    # the first four are probably the correct waveforms
	    set correct $samples
	    set map [dict create]
	    set i -1
	    foreach s $samples { dict lappend map $s [incr i] }
	    set hist [dict create 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0 10 0 11 0 12 0 13 0 14 0 15 0]
	    dict for {val posn} $map { dict incr hist [llength $posn] 1 }
	    puts "correct $ch $el [llength $correct] [dict size $map] histogram $hist"
	}
	dict set ::alldits $samples [dict create ch $ch el $el freq $freq pos $pos n $n patch [findpatch $ch $el $samples]]
    }

    # now get all the dits into one list
    set dits {}
    dict for {s dict} $::alldits {
	dict with dict {
	    set rec [list [lindex $pos 0] $ch $el $freq $n $patch]
	    lappend dits $rec
	}
    }

    # sort into order of first occurence, join into text
    set dits [join [concat [list [format-wave]] [lsort -index 0 -integer -increasing $dits]] \n]

    # insert into the text display if there has been a change
    if {$dits ne {} && $dits ne [.t get 0.0 end]} {
	.t replace 0.0 end $dits
    }
}

proc doplot {pos0 samples} {
    if { ! [winfo exists .plot]} {
	toplevel .p
	pack [canvas .p.c] -side top -fill both -expand true
    }
    
}

proc plot {w x y} {
    set i0 [$w index "@$x,$y linestart"]
    set i1 [$w index "@$x,$y lineend"]
    # 564649 q dah 1 9854 {0 {0 38} -48 {55 70} -16 {71 9853}}
    foreach {pos0 ch el fr len runs} [$w get $i0 $i1]
    dict for {samples dit} $::alldits {
	if {$pos0 == [lindex [dict get dit pos] 0]} {
	    doplot $pos0 $samples
	    return;
	}
    }
    # puts "plot $w $x $y [$w get $i0 $i1]"
}

proc main {args} {
    grid [text .t] -column 0 -row 1 -sticky nsew
    grid [scrollbar .v] -column 1 -row 1 -sticky ns
    grid [scrollbar .h -orient horizontal] -column 0 -row 2 -sticky ew
    grid columnconfigure . 0 -weight 1
    grid rowconfigure . 1 -weight 1
    .t configure -yscrollcommand [list .v set] -xscrollcommand [list .h set] -wrap none
    .v configure -command [list .t yview]
    .h configure -command [list .t xview]
    bind .t <Double-1> [list plot %W %x %y]
    after 500 [list process-inputs]
}

main
