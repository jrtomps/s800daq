
lappend auto_path [file join $::env(DAQROOT) TclLibs]

package provide ActionFilter 1.0

package require Actions
package require DefaultActions

package require RunstateMachine

namespace eval ActionFilter {
  variable pipe ""
  variable parser [::Actions %AUTO%]
  set ::DefaultActions::name "ActionFilter"
  

  proc register {} {
    set stateMachine [RunstateMachineSingleton %AUTO%]
    $stateMachine addCalloutBundle ActionFilter
    $stateMachine destroy
  }

  proc enter {from to} {
   puts "ActionFilter::enter $from $to"
  }

  proc leave {from to} {
   puts "ActionFilter::leave $from $to"
    if {($from eq "Halted") && ($to eq "Active")} {
      puts "startFilter"
      startFilter
    }

  }

  proc attach {state} {
    puts "ActionFilter::attach $state"
  }


  proc startFilter {} {
    variable pipe
    variable parser
    # kill off old filter if it still exists
    if {$pipe ne ""} {
      set pids [pid $pipe]
      foreach pid $pids { exec kill $pid }
    }

#    set cmd [file join /home tompkins actions slave filter UserFilter]
    set cmd [file join /user s800 converged_daq jorge filter UserFilter]
#    set pipe [open "| $cmd  --source=tcp://localhost/tompkins --sink=file:///dev/null --oneshot |& cat" r]
    set pipe [open "| $cmd  --source=tcp://localhost/s800built --sink=tcp://localhost/s800filter |& cat" r]
    chan configure $pipe -blocking 0
    chan configure $pipe -buffering line
    chan event $pipe readable [list $parser onReadable $pipe]
    puts "Filter is $pipe"
  }

  namespace export enter leave attach
}
