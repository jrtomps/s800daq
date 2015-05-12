# Event stack for S800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB VM0155

# Here we simply specify the event stack

$VMUSB ClearStack event
$VMUSB sAddMarker event 0xE800; # S800 VME crate tag
$VMUSB sReadScaler event A
$VMUSB sReadScaler event B


$VMUSB sAddMarker event 0x5803; # Time stamp packet
stamp sReadStamp event
$VMUSB sAddMarker event 0xf803; # Time stamp end packet

$VMUSB sAddMarker event 0xcfdc; # CRDC1 packet
#crdc1 sReadAll event
#$VMUSB sWait event 5;           # For some reason we have to wait 1 tick (200ns fixed) after a number read or it doesn't do repeat add markers correctly.
$VMUSB sAddMarker event 0x1111; # CRDC1 end packet

$VMUSB sAddMarker event 0x2222; # CRDC2 packet
$VMUSB sAddMarker event 0x3333; # CRDC2 packet
crdc2 sReadAll event
#$VMUSB sWait event 5
$VMUSB sAddMarker event 0xffdd; # CRDC2 end packet

$VMUSB sAddMarker event 0x5870; # PPAC packet
ppac sReadAll event
#$VMUSB sWait event 5
$VMUSB sAddMarker event 0xf870; # PPAC end packet
#$VMUSB sAddMarker event 0xadc1; # adc1 packet
#adc1 sRead event
