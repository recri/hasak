#!/usr/bin/tclsh

set ::skip_output_mixers 0

# process config.h to find NRPN parameters, types, etc.
# write a desymbolized #define list
# or write an eeprom read and write function
# or write a javascript dictionary

proc evaluate {expr values} {
    while {[regexp {(NOTE|CTRL|NRPN|KYR[A-Z]?)_[A-Z0-9_]+} $expr match]} {
	set expr [regsub $match $expr [dict get $values $match value]]
    }
    expr $expr
}

proc extract-type {cmt} {
    if {[regexp {^/\*\s+(bit|char|short|relocation|enumeration)\s+.*->.*\*/$} $cmt all type]} {
	return $type
    }
    return {}
}

proc extract-class {cmt} {
    if {[regexp {^/\*\s*([a-z]+)?\s+(unimpl|volatile|virtual)\s+.*->.*\*/$} $cmt all type class]} {
	return $class
    }
    return {}
}

proc extract-units {cmt} {
    if {[regexp {^/\*\s*([a-z]+)?\s*([a-z]+)?\s+(ms/10|µs|dits|x8k|dbdown|wpm|Hz|VAL_RAMP_\*|VAL_ADAPT_\*|VAL_KEYER_\*).*->.*\*/$} $cmt all type class unit]} {
	return $unit
    }
    return {}
}

set jsquoted 0

proc jsq {name} { if {$::jsquoted} { return "\"$name\"" } else { return $name } }

proc jsformat-par {name table} {
    dict with table {
	# values:
	# range:
	# units:
	set vals {}
	lappend vals "[jsq nrpn]: $value"
	lappend vals "[jsq label]: \"$label\""
	if {[info exists range]} { lappend vals "[jsq range]: \[[join $range {, }]\]" }
	if {[info exists units]} { lappend vals "[jsq units]: \"$units\"" }
	if {[info exists values]} { lappend vals "[jsq values]: \[join [lmap v [dict keys $table $values] {string concat \" $v \"}] {, }]\]" }
	return "[jsq $name]: { [join $vals {, }] }"
    }
}
proc jsformat-val {name table} {
    dict with table {
	# , valueof: ${value-of}
	return "[jsq $name]: { [jsq value]: $value, [jsq label]: \"$label\" }"
    }
}
proc jsformat-rel {name table} {
    dict with table {
	return "[jsq $name]: { [jsq nrpn]: $value, [jsq label]: \"$label\" }"
    }
}
array set ::seen {type 1 sub 1 value 1 label 1 title 1 units 1 range 1 values 1 opts 1}
proc jsformat-any {name table} {
    set vals {}
    dict with table {
	lappend vals "[jsq type]: \"$type\""
	dict for {key value} $table {
	    switch $key {
		orig-value - value-of { continue }
		unit -
		units { lappend vals "[jsq unit]: \"$value\"" }
		value { lappend vals "[jsq value]: $value" }
		listen - ignore - property - valuesProperty - type { }
		default { 
		    if {$key ne {default}} { 
			if { ! [info exists ::seen($key)]} {
			    puts stderr "saw $key in $name"
			    set ::seen($key) 1
			}
			lappend vals "[jsq $key]: \"$value\"" 
		    }
		}
	    }
	}
    }
    return "\"$name\": {[join $vals {, }]}"
}

proc jsformat-all {values} {
    set kyr {}
    set kyra {}
    set kyrc {}
    set kyre {}
    set kyrn {}
    set kyrp {}
    set prop {}
    set nrpn {}
    set kyrv {}
    # set pats [dict create]
    dict for {key table} $values {
	dict with table {
	    switch -regexp $key {
		^KYR_.*$ { 
		    # skip the default enable values
		    # skip the pin definitions which move around between 3.x and 4.x
		    switch -glob $key {
			KYR_MAGIC - KYR_ENABLE_TX - KYR_ENABLE_ST - KYR_ENABLE_POTS - KYR_ENABLE_WINKEY -
			"KYR_DIN" - "KYR_DOUT" - "KYR_MCLK" - "KYR_BCLK" - "KYR_LRCLK" -
			"KYR_SCL" - "KYR_SDA" - "KYR_MQSR" - "KYR_MQSL" - "KYR_ADC0" -
			"KYR_ADC*" - "KYR_DAC*" -
			"KYR_VOLUME_POT" - "KYR_ST_VOL_POT" - "KYR_ST_FREQ_POT" - "KYR_SPEED_POT" -
			"KYR_R_PAD_PIN" - "KYR_L_PAD_PIN" - "KYR_S_KEY_PIN" - "KYR_EXT_PTT_PIN" -
			"KYR_KEY_OUT_PIN" - "KYR_PTT_OUT_PIN" { continue }
			default { lappend kyr [jsformat-any $key $table] }
		    }
		}
		^NOTE_.*$ { lappend kyrn [jsformat-any $key $table] }
		^CTRL_.*$ { lappend kyrc [jsformat-any $key $table] }
		^NRPN_.*$ { 
		    if {$::skip_output_mixers} {
			# skip the output mixers
			if {[string match NRPN_MIX_USB* $key]} continue
			if {[string match NRPN_MIX_I2S* $key]} continue
			if {[string match NRPN_MIX_HDW* $key]} continue
			if {[string match NRPN_MIX_EN_* $key]} continue
		    }
		    lappend kyrp [jsformat-any $key $table]
		    if {$type in {nrpn opts}} {
			lappend prop "[jsq $property]: \"$key\""
		    }
		    if {$type in {nrpn}} {
			lappend nrpn "[jsq $value]: \"$key\""
		    }
		}
		^VAL_.*$ { lappend kyrv [jsformat-any $key $table] }
		^ENDP_.*$ { lappend kyre [jsformat-any $key $table] }
		^AUDIO_.*$ { lappend kyra [jsformat-any $key $table] }
		^SYS_EX.*$ continue
		default {
		    puts stderr "not matching \"$key\" in jsformat-all"
		}
	    }
	}
    }
    return [concat $kyr $kyra $kyrn $kyrc $kyrp $kyre $kyrv]; # leave off $prop $nrpn cross refs
}

proc jsformat {values} {
    set ::jsquoted 0
    set v [dict get $values KYR_VERSION value]
    set d ",\n    "
    return "// parameter map for hasak version $v
			// generated with .../hasak/doc/nrpn.tcl from .../hasak/config.h
			// do not edit, regenerated from .../hasak/config.h by .../hasak/doc/nrpn.tcl output js
			export const hasakProperties$v = {\n    [join [jsformat-all $values] $d]\n};"
}
proc json-minimize {values} {
    set min [dict create]
    set rest [dict create]
    set minnrpn [dict get $values NRPN_CHANNEL value]
    set maxnrpn [dict get $values NRPN_PIN value]
    dict for {key table} $values {
	dict with table {
	    switch -regexp $key {
		^KYR_VERSION$ { 
		    dict set min $key $table 
		    dict set rest $key $table 
		}
		^KYR_N_(NOTE|CTRL|NRPN)$ { dict set min $key $table }
		^KYR_.*$ continue
		^NOTE_MIDI_.*$ { dict set min $key $table }
		^NOTE_.*$ { dict set rest $key $table }
		^CTRL_.*$ { dict set min $key $table }
		^NRPN_CODEC_VOLUME$ { dict set min $key $table }
		^NRPN_INPUT_SELECT$ { dict set min $key $table }
		^NRPN_SET_DEFAULT$ { dict set min $key $table }
		^NRPN_ECHO_ALL$ { dict set min $key $table }
		^NRPN_ID_DEVICE$  { dict set min $key $table }
		^NRPN_ID_VERSION$ { dict set min $key $table }
		^NRPN_STRING_.*$ { dict set min $key $table }
		^NRPN_SPEED_FRAC$ { dict set rest $key $table }
		^NRPN_.*$ {
		    if {$type eq {rel}} { dict set rest $key $table
		    } elseif {$value >= $minnrpn && $value < $maxnrpn} { dict set min $key $table
		    } else  { dict set rest $key $table
		    }
		}
		^VAL_PADC_.*$  { dict set rest $key $table }
		^VAL_.*$ { dict set min $key $table }
		^ENDP_JSON_TO_HOST$ { dict set min $key $table }
		^ENDP_.*$  { dict set rest $key $table }
		^AUDIO_.*$ { dict set rest $key $table }
		^SYS_EX.*$ continue
		default {
		    puts stderr "not matching \"$key\" in json-minimize"
		}
	    }
	}
    }
    return [list $min $rest]
}

proc jsonformat {values} {
    set ::jsquoted 1
    set d ",\n    "
    return "{\n    [join [jsformat-all $values] $d]\n}"
}

proc c-string-escape {str} {
    set dq \"
    return [regsub -all $dq $str \\$dq]
}

proc jsoninc {values {suffix {}}} {
    set ::jsquoted 1
    set vals [jsformat-all $values]
    set vals [lmap v $vals {if { ! [regexp {^\"(NOTE_|CTRL_|NRPN_|VAL_|KYR_|ENDP_)} $v]} continue; set v}]
    set vals [lmap v $vals {c-string-escape $v}]
    set d ",\"\n    \""
    set vals [join $vals $d]
    return "// parameter map for hasak [dict get $values KYR_VERSION value] generated by \"hasak/scripts/nrpn.tcl output json-in-c\" from hasak/config.h
			// do not edit
			static const char json_string$suffix\[\] =\n    \"{\"\n    \"$vals\"\n    \"}\";
			// do not edit"
}

proc main {argv} {
    array set options {output js}
    if {([llength $argv]%2) != 0 ||
	[catch {array set options $argv} error]} {
	puts {usage nrpn.tcl [output (c|js|gaps|size|dups)]}
	exit 1
    }
    foreach f {config.h ../config.h} {
	if {[file exists $f]} {
	    break
	}
    }
    set fp [open $f]
    set data [read $fp]
    close $fp
    set vwid 0
    set size [dict create]
    set lines [dict create]
    set values [dict create]
    set globs {}
    foreach line [split [string trim $data] \n] {
	dict set lines [dict size $lines] $line
	if { ! [regexp {^#define[ \t]+([A-Z0-9_]+)[ \t]+([-('A-Za-z_0-9*+)]*)[ \t]*/\*[ \t]*{(.*)}[ \t]*\*/[ \t]*$} $line all name value comment]} {
	    switch -regexp $line {
		^$ continue
		{^[ \t]*$} continue
		{^/\*.*\*/$} continue
		{^/\* *$} continue
		{^ *\*/$} continue
		{^ \*.*$} continue
		{^\*\*.*$} continue
		{^#(ifdef|ifndef|if|else|elif|endif|error).*$} continue
		{^#define config_h_$} continue
		{^#define TEENSY.*$} continue
		{^#define EEPROM.*$} continue
		{^#define VAL_SIGN.*$} continue
		{^static.*$} continue
		{^//.*$} continue
		{^[ \t]+/\* xnrpn \*/$} continue
		default {
		    puts "mismatch: $line"
		    continue
		}
	    }
	}
	set vwid [expr {max($vwid, [string length $name])}]
	# dict set values $name name $name
	if {[catch {dict set values $name value [evaluate $value $values]} error]} {
	    error "evaluating line $line: $::errorInfo"
	}
	dict set values $name orig-value $value 
	#if { ! [regexp {/\*\s*{(.*)}\s*\*/} $rest all comment]} {
	#    puts "missing comment1 {$line}"
	#    continue
	#}
	if {[catch {
	    if {([llength $comment]%2) == 0} {
		foreach {key val} $comment {
		    dict set values $name $key $val
		    set rest {}
		}
	    }
	} error]} {
	    puts "malformed comment2 {$line}"
	    continue
	}
	
	if {$rest ne {}} {
	    dict set values $name comment $rest
	}
	foreach key [dict keys [dict get $values $name]] {
	    dict incr freq $key
	}
	if { ! [dict exists $values $name type]} {
	    puts "missing type {$line}"
	}
	if { ! [dict exists $values $name title]} {
	    puts "missing title {$line}"
	}
	if {[dict exists $values $name lable]} {
	    puts "found a lable {$line}"
	}
	if {[dict exists $values $name values]} {
	    set glob [dict get $values $name values]
	    set prop [dict get $values $name property]
	    if {[dict exists $values $name valuesProperty]} {
		set vproperty [dict get $values $name valuesProperty]
	    } else {
		set vproperty "${prop}s"
	    }
	    
	    # dict set values $glob name $glob
	    dict set values $glob type opts
	    dict set values $glob property $vproperty
	    dict set values $glob opts {}
	    lappend globs $glob
	}
	#dict set values $name type [extract-type $rest]
	# dict set values $name class [extract-class $rest]
	# dict set values $name units [extract-units $rest]
	# dict incr size [dict get $values $name type] 
    }
    # add in the duplicated keyer voice instances
    if {0} {
	set kmin [dict get $values NRPN_VOX_NONE value]
	set kmax [dict get $values NRPN_VOX_TUNE value]
	#puts "min $kmin max $kmax"
	foreach block [lrange [dict keys $values NRPN_VOX_*] 1 end] {
	    foreach name [dict keys $values NRPN_*] {
		set v [dict get $values $name value]
		#set c [dict get $values $name class]
		#set t [dict get $values $name type]
		if {$v >= $kmin && $v < $kmax} {
		    #dict incr size $t
		}
	    }
	}
    }
    # add in the value globs
    foreach glob $globs {
	# the glob itself matches the glob, but it's inserted before the values
	dict set values $glob opts [lrange [dict keys $values $glob] 1 end]
    }
    switch $options(output) {
	{c} {
	    dict for {name table} $values {
		if {! [dict exists $table value]} {
		    puts [format "#define %-${vwid}s %s %s" $name {/* no value */} "/* {$table} */"]
		} else {
		    puts [format "#define %-${vwid}s %3d %s" $name [dict get $table value] "/* {$table} */"]
		}
	    }
	}
	{js} { puts [jsformat $values] }
	{json-in-c} { puts [jsoninc $values] }
	{min-json-in-c} {
	    foreach {min rest} [json-minimize $values] break;
	    puts [jsoninc $min]
	    puts [jsoninc $rest 2]
	}
	{json} { puts [jsonformat $values] }
	all {
	    dict for {name table} $values {
		puts "$name $table"
	    }
	}
	size {
	    puts $size 
	}
	dups {
	    set dups [dict create]
	    foreach key [dict keys $values NRPN_*] {
		if {[dict get $values $key type] eq {nrpn}} {
		    dict lappend dups [dict get $values $key value] $key
		}
	    }
	    dict for {key dups} $dups {
		if {[llength $dups] > 2} { puts "dups on $key {$dups}" }
	    }
	}
	rel {
	    dict for {key table} $values {
		if {[dict get $values $key type] eq {rel}} {
		    puts "$key $table"
		}
	    }
	}
	props {
	    dict for {key table} $values {
		if {[dict get $table type] eq {nrpn}} {
		    puts "$key [dict get $table property]"
		}
	    }
	}
	gaps {
	    set cursor 0
	    dict for {name table} $values {
		dict with table {
		    if {$type in {nrpn rel}} {
			if {$value < $cursor} {
			    if {[string match NRPN_*VOX_OFFSET $name]} continue
			    if {$name in {NRPN_VOX_NONE}} {
				set cursor $value
				continue
			    }
			    puts "back track at $name=$value when cursor=$cursor"
			    continue
			} elseif {$value <= $cursor + 1} {
			    set cursor $value
			    continue
			} elseif {$value >= 1000} {
			    continue
			} else {
			    if {$name in {NRPN_MORSE NRPN_MIXER NRPN_VOX_NONE}} {
				set cursor $value
			    } elseif {$name in {NRPN_VOX_TUNE NRPN_VOX_S_KEY NRPN_VOX_PAD NRPN_VOX_WINK NRPN_VOX_KYR NRPN_VOX_BUT NRPN_LAST} &&
				      $value == $cursor+8} {
				set cursor $value
			    } else {
				puts "jump forward at $name=$value when cursor=$cursor"
			    }
			}
		    }
		}
	    }
	}
	none {
	}
	values {
	    set patterns [dict create]
	    dict for {key table} $values {
		set type [dict get $table type]
		if {$type eq {nrpn} && [dict exists $table values]} {
		    set vglob [dict get $table values]
		    if {[dict exists $patterns $vglob]} continue
		    dict set patterns $vglob true
		    set property [dict get $table property]
		    if {[dict exists $table valuesProperty]} {
			set vproperty [dict get $table valuesProperty]
		    } else {
			set vproperty "${property}s"
		    }
		    set voptions [lrange [dict keys $values $vglob] 1 end]
		    set vlabels [lmap x $voptions {dict get $values $x label}]
		    puts "$vglob $vproperty {$voptions} {$vlabels}"
		}
	    }
	}
	default {
	    dict for {name table} $values {
		puts [list [dict get $table type] [dict get $table class] [dict get $table units] $name [dict get $table value] [dict get $table comment]]
	    }
	    puts $size
	}
    }
}

main $argv
