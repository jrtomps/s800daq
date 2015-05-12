# Scaler stack for S800 DAQ
# CCUSB controller serial number is CC0105

set CCUSB CC0105

# Here we simply fill up the event stack and then load it into the controller

$CCUSB ClearStack scaler
$CCUSB sAddMarker scaler 0xC800; # S800 Camac crate tag

$CCUSB sAddMarker scaler 0x4434; # 4434 tag
scaler sReadAll scaler

$CCUSB sAddMarker scaler 0x2367; # ULM timestamp tag
trigger sStamp scaler;           # Read timestamp


