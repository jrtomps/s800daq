# Scaler stack for S800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB VM0079

# Here we simply fill up the event stack and then load it into the controller

$VMUSB ClearStack scaler
$VMUSB sAddMarker scaler 0xE800; # S800 VME crate tag

$VMUSB sAddMarker scaler 0x1234; # 1234 dummy tag
