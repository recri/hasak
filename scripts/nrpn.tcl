#!/usr/bin/tclsh

# process config.h to find NRPN parameters, types, etc.
# write a desymbolized #define list
# or write an eeprom read and write function
# or write a javascript dictionary

proc evaluate {expr values} {
    while {[regexp {KYR[A-Z]?_[A-Z0-9_]+} $expr match]} {
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
    if {[regexp {^/\*\s*([a-z]+)?\s*([a-z]+)?\s+(ms/10|µs|dits|x8k|dbdown|wpm|Hz|KYRV_RAMP_\*|KYRV_ADAPT_\*|KYRV_KEYER_\*).*->.*\*/$} $cmt all type class unit]} {
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
proc jsformat-any {name table} {
    set vals {}
    dict for {key value} $table {
	switch $key {
	    orig-value - value-of { continue }
	    units { lappend vals "[jsq unit]: \"$value\"" }
	    value { lappend vals "[jsq nrpn]: $value" }
	    default { lappend vals "[jsq $key]: \"$value\"" }
	}
    }
    return "\"$name\": {[join $vals {, }]}"
}

proc jsformat-all {values} {
    set kyrp {}
    set prop {}
    set nrpn {}
    set pats [dict create]
    dict for {key table} $values {
	dict with table {
	    # skip the extended NRPNs
	    if {[string match KYRP_X* $key]} continue
	    # skip the output mixers
	    if {[string match KYRP_MIX_* $key]} continue
	    # primary KYR* string -> Object table
	    if {$type in {rel par cmd inf val opts} ||
		[string match *VOX* $key] ||
		[string match *_CC_* $key] ||
		[string match KYRC_VERSION $key]
	    } {
		lappend kyrp [jsformat-any $key $table]
	    }
	    # property name -> KYR* table
	    if {$type in {par opts cmd inf}} {
		lappend prop "[jsq $property]: \"$key\""
	    }
	    # nrpn -> KYR* table
	    if {$type in {par cmd inf}} {
		lappend nrpn "[jsq $value]: \"$key\""
	    }
	}
    }
    return [concat $kyrp $prop $nrpn]
}
proc jsformat {values} {
    set ::jsquoted 0
    set v [dict get $values KYRC_VERSION value]
    set d ",\n    "
    return "// parameter map for hasak 100
// generated with .../hasak/doc/nrpn.tcl from .../hasak/config.h
// do not edit, regenerated from .../hasak/config.h by .../hasak/doc/nrpn.tcl output js
export const hasakProperties = {\n    [join [jsformat-all $values] $d]\n};"
}

proc jsonformat {values} {
    set ::jsquoted 1
    set v [dict get $values KYRC_VERSION value]
    set d ",\n    "
    return "{\n    [join [jsformat-all $values] $d]\n}"
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
	if { ! [regexp {^#define[ \t]+(KYR[A-Z]?_[A-Z0-9_]*)[ \t]+([^ \t]*)([ \t]+(.*))?$} $line all name value rest1 rest]} {
	    # puts "mismatch: $line"
	    continue
	}
	set vwid [expr {max($vwid, [string length $name])}]
	# dict set values $name name $name
	dict set values $name value [evaluate $value $values]
	dict set values $name orig-value $value 
	if { ! [regexp {/\*\s*{(.*)}\s*\*/} $rest all comment]} {
	    puts "missing comment1 {$line}"
	    continue
	}
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

	    dict set values $glob name $glob
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
    set kmin [dict get $values KYRP_VOX_NONE value]
    set kmax [dict get $values KYRP_VOX_TUNE value]
    #puts "min $kmin max $kmax"
    foreach block [lrange [dict keys $values KYRP_VOX_*] 1 end] {
	foreach name [dict keys $values KYRP_*] {
	    set v [dict get $values $name value]
	    #set c [dict get $values $name class]
	    #set t [dict get $values $name type]
	    if {$v >= $kmin && $v < $kmax} {
		#dict incr size $t
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
	    foreach key [dict keys $values KYRP_*] {
		if {[dict get $values $key type] eq {par}} {
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
		if {[dict get $table type] eq {par}} {
		    puts "$key [dict get $table property]"
		}
	    }
	}
	gaps {
	    set cursor 0
	    dict for {name table} $values {
		dict with table {
		    if {$type in {par rel}} {
			if {$value < $cursor} {
			    if {[string match KYRP_*VOX_OFFSET $name]} continue
			    if {$name in {KYRP_VOX_NONE}} {
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
			    if {$name in {KYRP_MORSE KYRP_MIXER KYRP_VOX_NONE}} {
				set cursor $value
			    } elseif {$name in {KYRP_VOX_TUNE KYRP_VOX_S_KEY KYRP_VOX_PAD KYRP_VOX_WINK KYRP_VOX_KYR KYRP_VOX_BUT KYRP_LAST} &&
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
		if {$type eq {par} && [dict exists $table values]} {
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
