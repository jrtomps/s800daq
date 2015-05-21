# Event stack for S800 DAQ
# VMUSB controller serial number is VM0079

puts "Adding VM0079Event.tcl to event stack"

set VMUSB $::Globals::aController
set event $::Globals::aReadoutList

if {[lsearch [itcl::find objects] crdc1] == -1} {ACrdcXLM72 crdc1 $VMUSB 10}
if {[lsearch [itcl::find objects] crdc2] == -1} {ACrdcXLM72 crdc2 $VMUSB 13}
if {[lsearch [itcl::find objects] ppac] == -1} {APpacXLM72 ppac $VMUSB 16}

# Here we simply specify the event stack

# $event addDelay 1
$event addMarker $crdc1Start
crdc1 sReadAll $event
$event addMarker $crdc1Finish

# $event addDelay 1
$event addMarker $crdc2Start
crdc2 sReadAll $event
$event addMarker $crdc2Finish 

# $event addDelay 1
$event addMarker $ppacStart
ppac sReadAll $event
#$event addDelay 5
$event addMarker $ppacFinish

puts "Done"
