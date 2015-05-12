# Begin sequence for s800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB VM0079

# Create module instances if they don't exist
if {[lsearch [itcl::find objects] stamp] == -1} {ATimeStampXLM72 stamp $VMUSB 4}
if {[lsearch [itcl::find objects] delay] == -1} {AGD16XLM72 delay $VMUSB 7}
if {[lsearch [itcl::find objects] crdc1] == -1} {ACrdcXLM72 crdc1 $VMUSB 10}
if {[lsearch [itcl::find objects] crdc2] == -1} {ACrdcXLM72 crdc2 $VMUSB 13}
if {[lsearch [itcl::find objects] ppac] == -1} {APpacXLM72 ppac $VMUSB 16}
#if {[lsearch [itcl::find objects] cfd] == -1} {ACAENV812 cfd $VMUSB 0x880000}
#if {[lsearch [itcl::find objects] adc1] == -1} {AMesytecMADC32 adc1 $VMUSB 0x770000}
if {[lsearch [itcl::find objects] caen] == -1} {ACAENV288 caen $VMUSB 0x200000}
if {[lsearch [itcl::find objects] icshp] == -1} {ACAENN568B icshp caen 1}
if {[lsearch [itcl::find objects] hodoshp1] == -1} {ACAENN568B hodoshp1 caen 2}
if {[lsearch [itcl::find objects] hodoshp2] == -1} {ACAENN568B hodoshp2 caen 3}
#if {[lsearch [itcl::find objects] tdc] == -1} {ACAENV1290 tdc $VMUSB 0xaa0000}
#if {[lsearch [itcl::find objects] scaler] == -1} {AXLM72Scaler scaler $VMUSB 9}
if {[lsearch [itcl::find objects] mtdc] == -1} {AMesytecMTDC32 mtdc $VMUSB 0x40000000}

puts -nonewline "Executing begin sequence of VME crate controlled by $VMUSB... "
flush stdout

# Crate Controller initialization
$VMUSB SetBufferLength 13000
#$VMUSB SetBufferLength 1024
$VMUSB SetBufferMode 0
$VMUSB SetHeader 1
$VMUSB SetBulkBuffers 0
$VMUSB SetForceDump 1
$VMUSB SetTriggerDelay 110
$VMUSB SetScalerPeriod 0
$VMUSB SetScalerFrequency 2

#$VMUSB SetNIMs 0x00020600; # Set NIM O1 to Busy, NIM O2 to End of Event, input of scaler A to NIM I2 and input of scaler B to term of A
#$VMUSB SetNIMs 0x00230000; # Set input of scaler A to Event and input of scaler B to NIM I2
$VMUSB SetNIMs 0x00010600; # Set input of scaler A to NIM I1 and input of scaler B to term of A, O1=Busy, O2=End of Event
#$VMUSB SetNIMs 0x00210000; # Set input of scaler A to NIM I1 and input of scaler B to NIM I2
$VMUSB SetNIMs 0x80CC0000; # Enable and reset scaler A & B

# Modules initializations
crdc1 Init ~/operations/daq/usb/Configs/s800crdc1v.tcl CRDC1_PADS
crdc2 Init ~/operations/daq/usb/Configs/s800crdc2v.tcl CRDC2_PADS
ppac Init ~/operations/daq/usb/Configs/s800tppacv.tcl TRACKI_PADS
delay Init ~/operations/daq/usb/Configs/s800tdcdelayini.tcl TDCDELAY
# Temporarily removed to achieve continuous time stamp mode (Sam's experiment)
#stamp Init

#adc1 Init
#adc1 SetMultiEvent 0
#adc1 SetThresholds 40
puts -nonewline "IC... "
flush stdout
icshp Init ~/operations/daq/usb/Configs/s800shpini.tcl IC_SHP
puts -nonewline "Hodo1... "
flush stdout
hodoshp1 Init ~/operations/daq/usb/Configs/s800shpini.tcl Hodo_SHP1
puts -nonewline "Hodo2... "
flush stdout
hodoshp2 Init ~/operations/daq/usb/Configs/s800shpini.tcl Hodo_SHP2
mtdc Init
mtdc SetMultiEvent 0
mtdc SetBank0Start 15384; # -1000 ns (16384 corresponds to 0)
#mtdc SetBank0Start 15684; # -700 ns (16384 corresponds to 0)
mtdc SetBank0Width 2000; # Window with (ns)
mtdc SetMultiHit 2; # set bank 0 to accept multihits
mtdc ResetFIFO
puts "Done."
flush stdout
