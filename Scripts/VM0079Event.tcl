# Event stack for S800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB VM0079

# Here we simply specify the event stack

$VMUSB ClearStack event
$VMUSB sAddMarker event 0xE800; # S800 VME crate tag
$VMUSB sReadScaler event A
$VMUSB sReadScaler event B


$VMUSB sAddMarker event 0x5803; # Time stamp packet
stamp sReadStamp event
$VMUSB sAddMarker event 0xf803; # Time stamp end packet

#$VMUSB sWait event 1
$VMUSB sAddMarker event 0xcfdc; # CRDC1 packet
crdc1 sReadAll event
$VMUSB sAddMarker event 0xffdc; # CRDC1 end packet

#$VMUSB sWait event 1
$VMUSB sAddMarker event 0xcfdd; # CRDC2 packet
crdc2 sReadAll event
$VMUSB sAddMarker event 0xffdd; # CRDC2 end packet

#$VMUSB sWait event 1
#$VMUSB sAddMarker event 0x5870; # PPAC packet
#ppac sReadAll event
#$VMUSB sWait event 5
#$VMUSB sAddMarker event 0xf870; # PPAC end packet

#$VMUSB sAddMarker event 0xadc1; # adc1 packet
#adc1 sRead event

$VMUSB sAddMarker event 0x0ddc; # Mesytec TDC packet
mtdc sRead event
$VMUSB sAddMarker event 0xfddc; # Mesytec TDC end packet
