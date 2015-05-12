# Event stack for S800 DAQ
# CCUSB controller serial number is CC0105

set CCUSB CC0105

# Here we simply fill up the event stack and then load it into the controller

$CCUSB ClearStack event
$CCUSB sAddMarker event 0xC800; # S800 Camac crate tag
$CCUSB sReadScaler event A
$CCUSB sReadScaler event B

$CCUSB sAddMarker event 0x2367; # 2367 tag
trigger sRead event; # Read trigger bits
trigger sStamp event; # Read time stamp
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf367; # 2367 end tag

$CCUSB sAddMarker event 0x4300; # 4300B tag
scint sReadSparse event; # Read scintillator energies
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf300; # 4300B end tag

$CCUSB sAddMarker event 0x7164; # 7164 tag
ionch sReadSparseLAM event; # Read ion chamber energies
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf164; # 7164 end tag

$CCUSB sAddMarker event 0x4448; # 4448 tag
hodopat sRead event A; # Read hodoscope hit pattern
hodopat sRead event B; # Read hodoscope hit pattern
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf448; # 4448 end tag

$CCUSB sAddMarker event 0x7165; # 7165 tag
hodo1 sReadSparseLAM event; # Read hodoscope energies (1-16)
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf165; # 7165 end tag

$CCUSB sAddMarker event 0x7166; # 7166 tag
hodo2 sReadSparseLAM event; # Read hodoscope energies (17-32)
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf166; # 7166 end tag

$CCUSB sAddMarker event 0x7167; # 7167 tag
s800adc sReadSparseLAM event; # Read S800 ADC
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf167; # 7167 end tag

$CCUSB sAddMarker event 0x7186; # 7186 tag
s800tdc sReadSparseLAM event; # Read S800 TDC
#$CCUSB sWait event 1
$CCUSB sAddMarker event 0xf186; # 7186 end tag

#$CCUSB sAddMarker event 0x7187; # 7187 tag
#labrtdc sReadSparseLAM event; # Read LaBr3 TDC
#$CCUSB sWait event 1
#$CCUSB sAddMarker event 0xf187; # 7187 end tag


trigger sClearRegister event
scint sClear event
ionch sClear event
hodo1 sClear event
hodo2 sClear event
hodopat sClear event
s800adc sClear event
s800tdc sClear event
labrtdc sClear event
