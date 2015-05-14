# Event stack for S800 DAQ
# VMUSB controller serial number is VM0079

set event $::Globals::aReadoutList

# Here we simply specify the event stack

# $event addDelay 1
$event addMarker $crdc1StartTag
crdc1 sReadAll $event
$event addMarker $crdc2FinishTag

# $event addDelay 1
$event addMarker $crdc2Start
crdc2 sReadAll $event
$event addMarker $crdc2Finish 

# $event addDelay 1
$event addMarker $ppacStartTag
ppac sReadAll $event
#$event addDelay 5
$event addMarker $ppacFinishTag

