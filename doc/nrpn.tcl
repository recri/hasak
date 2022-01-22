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

proc jsformat-par {name table} {
    dict with table {
	# values:
	# range:
	# units:
	set vals {}
	lappend vals "nrpn: $value"
	lappend vals "label: \"$label\""
	if {[info exists range]} { lappend vals "range: \[[join $range {, }]\]" }
	if {[info exists units]} { lappend vals "units: \"$units\"" }
	if {[info exists values]} { lappend vals "values: \[join [lmap v [dict keys $table $values] {string concat \" $v \"}] {, }]\]" }
	return "$name: { [join $vals {, }] }"
    }
}
proc jsformat-val {name table} {
    dict with table {
	# , valueof: ${value-of}
	return "$name: { value: $value, label: \"$label\" }"
    }
}
proc jsformat-rel {name table} {
    dict with table {
	return "$name: { nrpn: $value, label: \"$label\" }"
    }
}
proc jsformat-any {name table} {
    set vals {}
    dict for {key value} $table {
	lappend vals "$key: \"$value\""
    }
    return $vals
}

proc jsformat-all {values} {
    set js {}
    dict for {name table} $values {
	lappend js [jsformat-any $name $table]
    }
    return $js
}
proc jsformat {values} {
    set v [dict get $values KYRC_VERSION value]
    set d ",\n    "
    return "const CWKeyerHasak$v = {\n  [join [jsformat-all $values] $d]\n};"
}

proc main {argv} {
    array set options {output c}
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
    foreach line [split [string trim $data] \n] {
	dict set lines [dict size $lines] $line
	if { ! [regexp {^#define[ \t]+(KYR[A-Z]?_[A-Z0-9_]*)[ \t]+([^ \t]*)([ \t]+(.*))?$} $line all name value rest1 rest]} {
	    # puts "mismatch: $line"
	    continue
	}
	set vwid [expr {max($vwid, [string length $name])}]
	dict set values $name orig-value $value 
	dict set values $name value [evaluate $value $values]
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
	if { ! [dict exists $values $name label]} {
	    puts "missing label {$line}"
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
    switch $options(output) {
	{c} {
	    dict for {name table} $values {
		puts [format "#define %-${vwid}s %3d %s" $name [dict get $table value] "/* {$table} */"]
	    }
	}
	{js} { puts [jsformat $values] }
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
	gaps {
	}
	none {
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
