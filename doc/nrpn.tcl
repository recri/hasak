#!/usr/bin/tclsh

proc evaluate {expr} {
    while {[regexp {KYRP_[A-Z0-9_]+} $expr match]} {
	set expr [regsub {KYRP_[A-Z0-9_]+} $expr [dict get $::valueof $match]]
    }
    expr $expr
}

set valueof [dict create]
set comment [dict create]
foreach f {config.h ../config.h} {
    if {[file exists $f]} {
	set fp [open $f]
	set data [read $fp]
	close $fp
	set vwid 0
	foreach line [split [string trim $data] \n] {
	    if {[regexp {^#define[ \t]+(KYRP_[A-Z0-9_]*)[ \t]+([^ \t]*)([ \t]+(.*))?$} $line all name value rest1 rest]} {
		dict set ::valueof $name [evaluate $value]
		dict set ::comment $name $rest
		set vwid [expr {max($vwid, [string length $name])}]
		if {$name eq {KYRP_LAST}} break
	    }
	}
	dict for {name val} $::valueof {
	    puts [format "#define %-${vwid}s %3d %s" $name $val [dict get $::comment $name]]
	}
    }
}
