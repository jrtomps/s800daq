set here [file dirname [file normalize [info script]]]
lappend auto_path [file join $::env(DAQROOT) TclLibs]
lappend auto_path $here

package require evbcallouts 
package require ActionFilter

ActionFilter::register

::EVBC::useEventBuilder

::EVBC::registerRingSource tcp://spdaq48/rawccusb libdummytstamp.so 0  "CCUSB"
::EVBC::registerRingSource tcp://spdaq48/rawvmusb libdummytstamp.so 1  "VMUSB"

proc OnStart {} {
	::EVBC::initialize -restart false -glombuild true -glomdt 10 -destring s800built
}
