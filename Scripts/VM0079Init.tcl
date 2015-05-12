# Init sequence for s800 DAQ
# VMUSB controller serial number is VM0079

set VMUSB $::Globals::aController

# Create module instances if they don't exist

# uses the XLMTimestamp driver instead
#if {[lsearch [itcl::find objects] delay] == -1} {AGD16XLM72 delay $VMUSB 7}
#if {[lsearch [itcl::find objects] crdc1] == -1} {ACrdcXLM72 crdc1 $VMUSB 10}
if {[lsearch [itcl::find objects] crdc2] == -1} {ACrdcXLM72 crdc2 $VMUSB 2}
#if {[lsearch [itcl::find objects] crdc2] == -1} {ACrdcXLM72 crdc2 $VMUSB 13}
#if {[lsearch [itcl::find objects] ppac] == -1} {APpacXLM72 ppac $VMUSB 16}

#if {[lsearch [itcl::find objects] mtdc] == -1} {AMesytecMTDC32 mtdc $VMUSB 0x40000000}

# Configuring the XLM72V modules
# puts "Initializing crdc1 XLM72V"
#crdc1 Init /user/s800/operations/daq/usb/Configs/s800crdc1v.tcl CRDC1_PADS
#crdc1 AccessBus 1
#for {set i 0} {$i < 100} {incr i 4} {crdc1 Write srama $i 0}
#crdc1 ReleaseBus

puts "Initializing crdc2 XLM72V."
crdc2 Init /user/s800/operations/daq/usb/Configs/s800crdc2v.tcl CRDC2_PADS
crdc2 AccessBus 1
for {set i 0} {$i < 100} {incr i 4} {crdc2 Write srama $i 0}
crdc2 ReleaseBus

#puts "Please wait. Writing firmware to ppac XLM72V module FPGA."
#ppac Configure $ppac(firmware)
#after 500
#ppac Init ~/operations/daq/usb/Configs/s800tppacv.tcl TRACKI_PADS
#
#puts "Please wait. Writing firmware to delay XLM72 module FPGA."
#delay Configure $VMUSB $delay(firmware)
#after 500
#delay Init $VMUSB ~/operations/daq/usb/Configs/s800tdcdelayini.tcl TDCDELAY
## Now start the GUI if it isn't up yet
#if {![winfo exists .gd16]} {
#	toplevel .gd16
#	delay SetupGUI .gd16
#	delay UpdateGUIGD16
#}
#
#puts "Please wait. Configuring Mesytec MTDC32. Firmware version: [mtdc GetFirmware]."
#mtdc Init
