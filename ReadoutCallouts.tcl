set here [file dirname [file normalize [info script]]]
lappend auto_path [file join $::env(DAQROOT) TclLibs]
lappend auto_path $here

package require evbcallouts 
package require ActionFilter
package require ReadoutGuiRemoteControl


ActionFilter::register

::EVBC::useEventBuilder

::EVBC::registerRingSource tcp://spdaq19/rawccusb libdummytstamp.so 0  "CCUSB"
::EVBC::registerRingSource tcp://spdaq19/rawvmusb libdummytstamp.so 1  "VMUSB"

RemoteControlClient::initialize

proc launchULMGUI {} {
    exec $::env(DAQBIN)/ULMTriggerGUI --slot 17 --port 27001 --host localhost --module remoteCCUSBHandler --ring rawccusb &
}

proc createULMLauncher {} {
	ttk::frame .ulmFrame
	ttk::button .ulmFrame.launchBtn -text "Launch ULM GUI" -command launchULMGUI

	grid .ulmFrame.launchBtn -sticky nsew
	grid rowconfigure .ulmFrame 0 -weight 1
	grid columnconfigure .ulmFrame 0 -weight 1

	set dimensions [grid size .]
	set rows [lindex $dimensions 1]
	set cols [lindex $dimensions 0]
	incr rows

	grid .ulmFrame -row $rows -column 0 -columnspan $cols -sticky nsew -padx 5 -pady 3
}

proc OnStart {} {
	::EVBC::initialize -restart false -glombuild true -glomdt 10 -destring s800built
	createULMLauncher
}
