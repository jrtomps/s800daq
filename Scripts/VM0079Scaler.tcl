# Scaler stack for S800 DAQ
# VMUSB controller serial number is VM0079

set sclrStack $::Globals::aReadoutList

# Here we simply fill up the event stack and then load it into the controller

$sclrStack addMarker 0xE800; # S800 VME crate tag

$sclrStack addMarker 0x1234; # 1234 dummy tag
