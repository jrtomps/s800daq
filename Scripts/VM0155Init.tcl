# Init sequence for s800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB VM0155
set crdc(firmware) ~/server/fpga/crdc5v.bit
set ppac(firmware) ~/server/fpga/ppac2v.bit
set stamp(firmware) ~/server/fpga/stamp64.bit
set delay(firmware) ~/server/fpga/gd16.bit

# Create module instances if they don't exist
if {[lsearch [itcl::find objects] stamp] == -1} {ATimeStampXLM72 stamp $VMUSB 2}
if {[lsearch [itcl::find objects] delay] == -1} {AGD16XLM72 delay $VMUSB 4}
if {[lsearch [itcl::find objects] crdc1] == -1} {ACrdcXLM72 crdc1 $VMUSB 6}
if {[lsearch [itcl::find objects] crdc2] == -1} {ACrdcXLM72 crdc2 $VMUSB 8}
if {[lsearch [itcl::find objects] ppac] == -1} {APpacXLM72 ppac $VMUSB 10}

# Configuring the XLM72V modules
puts "Please wait. Writing firmware to crdc1 XLM72V module FPGA."
crdc1 Configure $crdc(firmware)
after 500
crdc1 Init ~/operations/daq/usb/Configs/s800crdc1v.tcl CRDC1_PADS
crdc1 AccessBus 1
for {set i 0} {$i < 100} {incr i 4} {crdc1 Write srama $i 0}
crdc1 ReleaseBus

puts "Please wait. Writing firmware to crdc2 XLM72V module FPGA."
crdc2 Configure $crdc(firmware)
after 500
crdc2 Init ~/operations/daq/usb/Configs/s800crdc2v.tcl CRDC2_PADS
crdc2 AccessBus 1
for {set i 0} {$i < 100} {incr i 4} {crdc2 Write srama $i 0}
crdc2 ReleaseBus

puts "Please wait. Writing firmware to ppac XLM72V module FPGA."
ppac Configure $ppac(firmware)
after 500
ppac Init ~/operations/daq/usb/Configs/s800tppacv.tcl TRACKI_PADS

puts "Please wait. Writing firmware to stamp XLM72 module FPGA."
stamp Configure $stamp(firmware)
after 500
stamp Init

puts "Please wait. Writing firmware to delay XLM72 module FPGA."
delay Configure $delay(firmware)
after 500
delay Init ~/operations/daq/usb/Configs/s800tdcdelayini.tcl TDCDELAY
# Now start the GUI if it isn't up yet
if {![winfo exists .gd16]} {
	toplevel .gd16
	delay SetupGUI .gd16
	delay UpdateGUIGD16
}
