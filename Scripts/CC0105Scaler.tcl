# Scaler stack for S800 DAQ
# CCUSB controller serial number is CC0105

set scaler $::Globals::aReadoutList

# Here we simply fill up the event stack and then load it into the controller

$CCUSB sAddMarker scaler 0x2367; # ULM timestamp tag
trigger sStamp scaler;           # Read timestamp


