
lappend auto_path [file join $::env(DAQROOT) TclLibs]

package provide ActionFilter 1.0

package require Actions
package require DefaultActions

package require RunstateMachine
package require ring

namespace eval ActionFilter {
  variable pipe {}
  variable parser [::Actions %AUTO%]
  set ::DefaultActions::name "ActionFilter"
  

  proc register {} {
    set stateMachine [RunstateMachineSingleton %AUTO%]
    $stateMachine addCalloutBundle ActionFilter
    $stateMachine destroy
  }

  proc enter {from to} {
  }

  proc leave {from to} {
    if {($from eq "Halted") && ($to eq "Active")} {
      startFilter
#      exec xterm -e /user/s800/converged_daq/jorge/filter/launchFilter &
    }

  }

  proc attach {state} {
  }


  proc killOldProvider {ringName} {
        if {$ringName in [ringbuffer list]} {
		set usage [ringbuffer usage $ringName]
		set producerPid [lindex $usage 3]
		if {$producerPid != -1} {
			ReadoutGUIPanel::Log ActionFilter warning "Killing off old producing process with PID=$producerPid"			
			exec kill $producerPid
		}
	}
  }

  proc startFilter {} {
    variable pipe
    variable parser
    # on the first time running, kill of old processes, and launch a new one.
    if {$pipe eq {}} {
	killOldProvider s800filter
	set cmd [file join /user s800 converged_daq jorge filter UserFilter]
	set pipe [open "| $cmd  --source=tcp://localhost/s800built --sink=tcp://localhost/s800filter |& cat" r]
	chan configure $pipe -blocking 0
	chan configure $pipe -buffering line
	chan event $pipe readable [list $parser onReadable $pipe]
    }
  }

  namespace export enter leave attach
}
