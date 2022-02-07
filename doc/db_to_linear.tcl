proc db_to_linear {db} { expr {pow(10, $db/20.0)} }

proc range {v0 dv vn} {
    set v {}
    if {$v0 <= $vn && $dv > 0} {
	for {set x $v0} {$x <= $vn} {set x [expr {$x+$dv}]} { lappend v $x }
    } elseif {$v0 > $vn && $dv < 0} {
	for {set x $v0} {$x >= $vn} {set x [expr {$x+$dv}]} { lappend v $x }
    } 
    return $v
}

set one_octave [lmap x [range 0 1 23] {expr {[db_to_linear [expr {$x/4.0}]]}}]

proc qtr_db_to_linear {qdb} {
    set inoctave [expr {int($qdb%24)}]
    set octave [expr {int($qdb/24)}]
    if {$inoctave < 0} {
	incr inoctave 24
	incr octave -1
    }
    expr {[lindex $::one_octave $inoctave]*pow(2,$octave)}
}    
    
# foreach db [range -50 0.25 6] { puts [format {%4f %4f %4f} $db [qtr_db_to_linear [expr {int(4*$db)}]] [db_to_linear $db]] }
## puts [lmap db [range 0 -0.1 -60] {expr {int(127*[db_to_linear $db])}}]    
# generate tenthdbtolinearasbyte
set results {}
for {set db 0} {1} {set db [expr {$db-0.1}]} { 
    lappend results [expr {int(127*[db_to_linear $db])}]
    if {[lindex $results end] == 0} break
}
puts [join $results ", "]
